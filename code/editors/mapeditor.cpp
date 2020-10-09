

#include "mapeditor.hpp"
#include "../sys.hpp"


/*
	Todo - create button that will edit the currently selected tilesheet. 
	Use 
	Sys::GetContext()->enter(APP_TILESET_EDITOR, false) 
	 
*/
MapEditor::MapEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{
}

void MapEditor::onEnter()
{
	LOG("Entering map editor ... \n");
	Editor::onEnter();
	
	int screenw, screenh;
	Engine::GetSize(screenw, screenh);

	m_tilesetSelection = 0;
	m_map = Assets::Load<Graphics::Map>(m_mapName);

	m_tilepicker = new UI::TilePicker();
	const std::string &tilesetName = m_map ? m_map->tilesets[0] : "";
	m_tilepicker->reload( tilesetName );


	//use just to get w/h
	const std::string &fontName = DEFAULT_FONT;
	Graphics::Font * font = Assets::Load<Graphics::Font>(DEFAULT_FONT);
	const int charW = font->charW;
	const int charH = font->charH;
	Assets::Unload<Graphics::Font>(fontName);

	const int toolY = screenh - (m_tilepicker->rect().h + TILE_H);

	//TODO - default tile w and height
	m_tooldata = {0,0, TILE_W, TILE_H};
	m_tool = MAP_TOOL_PIXEL;

	//managed locally. is not added to asset manager
	m_overlay = new Graphics::Tileset("MapEditor_Overlay", screenw, screenh);
	Editor::addTool("PIXEL", [&](){
		m_tool = MAP_TOOL_PIXEL;                     
	});
	//TODO - fill tiles

	//m_toolbar->add("FILL", [&](){
	//	m_tool = MAP_TOOL_FILL;             
	//});

	Editor::addTool("ERASE", [&](){
		m_tool = MAP_TOOL_ERASE;
		m_tooldata = { -1, -1, -1, -1, -1 ,-1 };
	});

	int tw = screenw - 16* charW;
	m_tilesetSelectToolbar = new UI::Toolbar(this, tw, toolY);

	const int border = 2;
	for(size_t i = 0; i < TILESETS_PER_MAP; i++)
	{
		m_tilesetSelectToolbar->add(
			std::to_string(i), 
			[&, i](){
				this->useTileset( (m_tilesetSelection = i) );
			}, 
			false
		);
		tw+=(charW+border*2); //left and right border
	}	
	//add two buttons
	m_tilesetInput = new UI::TextInput(tilesetName, tw, toolY, tw, 1, fontName);
	m_tilesetInput->cbAccept = [this]()
	{
		this->setTileset(m_tilesetSelection, m_tilesetInput->text);
	};
	//first add toolbat	
	//add the ui widgets

	App::addWidget( m_tilepicker);
	App::addWidget( m_tilesetSelectToolbar );
	App::addButton( m_tilesetInput );
	//select pixel and first tileset by default
	m_tilesetInput->cbAccept();
	showWorkspace();
	useTileset( 0 );

}

void MapEditor::onExit()
{	
	delete m_overlay;
	m_overlay = 0;

	Assets::Unload<Graphics::Map>(m_mapName);
	//delete widgets
	App::clear();
	Editor::onExit();


	LOG("Exited map editor\n");
}



void MapEditor::onTick()
{
	Engine::GetMousePosition(m_tooldata.mx, m_tooldata.my);

	//if not scrolling, must be tool
	if( ! handleScroll() )
	{
		handleTool();
	}

	Engine::Clear(EDITOR_COLOR);

	if(m_map)
	{
		m_map->draw();
	}


	if(!m_workspaceHidden)
	{
		drawOverlay();
	}

}

bool MapEditor::handleScroll()
{
	//handle scroll
	MouseButton btn = m_shift ? MOUSEBUTTON_LEFT : MOUSEBUTTON_RIGHT;

	//scroll if necessary
	if ( Engine::GetMouseButtonState( btn ) == BUTTON_CLICK )
	{
		m_prevmx = m_tooldata.mx;
		m_prevmy = m_tooldata.my;
		return true;
	}
	else if ( Engine::GetMouseButtonState( btn ) == BUTTON_HOLD )
	{
		const int dx = m_prevmx - m_tooldata.mx;
		const int dy = m_prevmy - m_tooldata.my;
		m_map->scroll(dx, dy);
		m_prevmx = m_tooldata.mx;
		m_prevmy = m_tooldata.my;
		return true;
	}
	return false;
}



//
void MapEditor::onKey( Key key, ButtonState state)
{
	int mx, my;
	Engine::GetMousePosition(mx, my);
	const float scale = m_map ? m_map->scale() : 1.0;

	//offset in world
	const int offx = m_map ? m_map->view.x : 0.0;
	const int offy = m_map ? m_map->view.y : 0.0;

	const int pixelx = static_cast<int>(offx + mx*scale);
	const int pixely = static_cast<int>(offy + my*scale);

	if(key == KEY_SHIFT) 
	{  
		m_shift = state == BUTTON_CLICK;
		return;
	}	

	if ( state == BUTTON_RELEASE)
	{
		return;
	}
	switch(key)
	{
		case KEY_a:
			//zoom in
			m_map->zoomTo(1.f/2, pixelx , pixely);
			m_map->update();

		break;

		case KEY_s:
			//zoom in
			m_map->zoomTo(2.f, pixelx, pixely);
			m_map->update();
		break;
		case KEY_x:
			if(m_tool == MAP_TOOL_ERASE)
			{
				if(m_tooldata.w > 1 )
				{
					m_tooldata.w--;
					m_tooldata.h--;
				}
			}
		break;
		case KEY_z:
			if(m_tool == MAP_TOOL_ERASE)
			{
				if(m_tooldata.w < m_map->rect.w)
				{
					m_tooldata.w++;
					m_tooldata.h++;
				}
			}
		break;
		case KEY_h:
			if(m_workspaceHidden)
				showWorkspace();
			else
				hideWorkspace();
		break;
		default:
			m_tilepicker->handleKey(key, state);
		break;
	}

}

bool MapEditor::handleTool()
{
	if(!m_map ) return false;
	const Rect & selection = m_tilepicker->selection();
	const Graphics::Tileset * tileset = m_tilepicker->tileset();
	//if pixel tool
	switch(m_tool)
	{
		case MAP_TOOL_PIXEL:
			if (( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_CLICK )
				|| ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD ))
			{
				const Rect& cursor = m_map->getTileRect( m_tooldata.mx, m_tooldata.my );
				if ( cursor.w != -1 )
				{
					//set tile
					int tilex, tiley;
					const int tilew = m_map->tilew, tileh = m_map->tileh;

					//for each tile in selection
					for(int dx = 0; dx < selection.w; dx+=tilew)
					{
						for(int dy = 0; dy < selection.h; dy+=tileh)
						{	
							const int mapx = cursor.x+dx/m_map->scale();
							const int mapy = cursor.y+dy/m_map->scale();
							if ( m_map->getTileXY( mapx, mapy, tilex, tiley ) )
							{
								const int id = tileset->id({ 
										selection.x + dx, selection.y + dy, 
										m_map->tilew, m_map->tileh 
								});

								m_map->tiles[tiley * m_map->w + tilex] = id;
								m_map->update( { tilex, tiley, 1, 1 } );
							}
						}
					}
					return true;
					//
				}
			}
		break;
		case MAP_TOOL_ERASE:
			if (( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_CLICK )
				|| ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD ))
			{
				const Rect& cursor = m_map->getTileRect( m_tooldata.mx, m_tooldata.my );
				if ( cursor.w != -1 )
				{
					//set tile
					int tilex, tiley;
					if ( m_map->getTileXY( cursor.x, cursor.y, tilex, tiley ) )
					{
						m_map->tiles[tiley * m_map->w + tilex] = TILE_CLEAR;
						m_map->update( { tilex, tiley, 1, 1 } );
						return true;
					}
				}
			}
		break;

	}
	return false;
}

void MapEditor::drawOverlay()
{
	//if no tileset
	if ( !m_map || !m_overlay || !m_tilepicker->tileset()) return;

	//draw overlaw
	//tile in map texture		
	memset(m_overlay->pixels, 0,  m_overlay->w * m_overlay->h * sizeof(Color));
	m_overlay->update();

	const Rect& tile = m_map->getTileRect( m_tooldata.mx, m_tooldata.my );
	switch(m_tool)
	{
		case MAP_TOOL_PIXEL:
			if(tile.w != -1)
			{	
				const Rect & src = m_tilepicker->selection();
				const int texture = m_tilepicker->tileset()->texture;
				const Rect dest = 
				{ 
					(int)(tile.x), 
					(int)(tile.y), 
					(int)(src.w / m_map->scale()), 
					(int)(src.h / m_map->scale())
				};
				Engine::DrawTexture(texture, src, dest);
				Engine::DrawRect(BORDER_COLOR, dest, false);
				m_overlay->update(tile);
			}	
		break;
		case MAP_TOOL_ERASE:
		{
			if(tile.w != -1)
			{	
				Engine::DrawTextureRect( m_overlay->texture, HIGHLIGHT, tile, true);
				Engine::DrawTextureRect( m_overlay->texture, BORDER_COLOR, tile, false);
				m_overlay->update(tile);
			}	
		}
	}
	Engine::DrawTexture( m_overlay->texture, { 0,0,m_overlay->w, m_overlay->h }, m_map->rect );
}


void MapEditor::undo()
{ 	

}

void MapEditor::redo()
{
}

void MapEditor::save()
{
	if(!m_map) return;

	Assets::Save(m_map);
}

void MapEditor::setMap( const std::string& name )
{
	m_mapName = name;
}	//

void MapEditor::setTileset(int index,  const std::string& tileset )
{
	if ( !m_map ) return;
	m_map->tilesets[index] = tileset;
	useTileset( index );
}

void MapEditor::useTileset(int index )
{
	if ( !m_map ) return;
	const std::string tilesetName = m_map->tilesets[index];
	m_tilesetInput->setText(tilesetName ); 
	m_tilepicker->reload( tilesetName );
}

//toolbar and tilepicker. resize map
void MapEditor::hideWorkspace() 
{
	int w,h;
	Engine::GetSize(w,h);
	m_workspaceHidden = true;
	m_map->rect = { 0, TILE_H, w, h };
	//reset view
   	m_map->zoomTo(1.0, 0, 0 );
   	m_tilepicker->hidden = true;
	m_tilesetInput->hidden = true;
	m_tilesetSelectToolbar->hidden = true;
   	hideTools(true);
}

void MapEditor::showWorkspace()
{
	int w,h;
	Engine::GetSize(w,h);
	m_workspaceHidden = false;
	const int toolY = h - (m_tilepicker->rect().h + TILE_H);
	if ( m_map )
	{
		m_map->rect = { 0, TILE_H, w, toolY - TILE_H };
   		m_map->zoomTo(1.0, 0, 0 );
	}

   	m_tilepicker->hidden = false;
	m_tilesetInput->hidden = false;
	m_tilesetSelectToolbar->hidden = false;
   	hideTools(false);

}