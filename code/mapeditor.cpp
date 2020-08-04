#include "pch.hpp"

#include "mapeditor.hpp"


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
	int w,h;
	Engine::GetSize(w,h);
	int charH = Sys::GetShell()->getFont()->charH;
	int charW = Sys::GetShell()->getFont()->charW;


	m_tilepicker = new UI::TilePicker();
	m_map = Assets::Load<Graphics::Map>(m_mapName);
	//TODO - create combox box to select tilesets  
	const std::string tilesetName = m_mapName;
	setTileset( 0, tilesetName );	//by default

	const int toolY = h - (m_tilepicker->rect().h + TILE_H);

	m_map->rect = { 0, TILE_H, w, toolY - TILE_H };
	//reset view
   	m_map->zoomTo(1.0, 0, 0 );

	//TODO - text edit to set tileset
	m_tooldata = {0,0, m_map->tilew, m_map->tileh};
	m_tool = MAP_TOOL_PIXEL;

	m_overlay = new Graphics::Tileset("TilesetEditor_Overlay", m_map->rect.w, m_map->rect.h);
	m_toolbar = new UI::Toolbar(this, 0, toolY);

	m_toolbar->add("PIXEL", [&](){
		m_tool = MAP_TOOL_PIXEL;                     
	});
	
	//m_toolbar->add("FILL", [&](){
	//	m_tool = MAP_TOOL_FILL;             
	//});

	m_toolbar->add("ERASE", [&](){
		m_tool = MAP_TOOL_ERASE;
		m_tooldata = { -1, -1, -1, -1, -1 ,-1 };
	});

	const int tw = 12;
	m_tilesetInput = new UI::TextInput(tilesetName, w - tw*charW, toolY, tw, 1);

	m_tilesetInput->cbAccept = [this]()
	{
		this->setTileset(0, m_tilesetInput->text);
	};

	//first add toolbat	
	//add the ui widgets
	App::addWidget( m_toolbar );

	App::addWidget(m_tilepicker);
	App::addButton( m_tilesetInput );

	m_toolbar->get(MAP_TOOL_PIXEL)->click();

	Editor::onEnter();


/////////////// DEbug //////////////////////////
	// const int spriteId =8 ;
	// for(int y = SPRITE_H; y < m_map->rect.h; y+=SPRITE_H*2)
	// {
	// 	for(int x = SPRITE_W; x < w; x+=SPRITE_W*2)
	// 	{	
	// 		Graphics::Sprite * sprite = new Graphics::Sprite( spriteId );
	// 		sprite->tileset = m_map->tilesets[0];
	// 		sprite->x = x;
	// 		sprite->y = y;
	// 		sprite->reload();
	// 		m_sprites.push_back(sprite);
	// 	}
	// }
///////////////////////////////////////////////////
}

void MapEditor::onExit()
{	
	delete m_overlay;
	m_overlay = 0;

	for(Graphics::Sprite * sprite : m_sprites )
		delete sprite;
	
	m_sprites.clear();
	
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

	Engine::ClearScreen(EDITOR_COLOR);

	if(m_map)
	{
		m_map->draw();
	}

	// for(Graphics::Sprite * sprite : m_sprites )	
	// {
	// 	sprite->draw();
	// }	

	drawOverlay();

}

bool MapEditor::handleScroll()
{
	//handle scroll
	MouseButton btn = m_shift ? MOUSEBUTTON_LEFT : MOUSEBUTTON_RIGHT;

	//scroll if necessary
	if ( Engine::GetMouseButtonState( btn ) == BUTTON_DOWN )
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
void MapEditor::onKey( Key key, bool isDown )
{
	const Rect& view = m_map->view;
	int mx, my;
	Engine::GetMousePosition(mx, my);
	//offset in world
	const int pixelx = view.x + ( mx - view.x )  ;
	const int pixely = view.y + ( my - view.y ) ;


	if(key == KEY_SHIFT) 
	{  
		m_shift = isDown;
		return;
	}	

	if ( !isDown )
	{
		return;
	}
	switch(key)
	{

		case KEY_MINUS:
			//zoom in
			m_map->zoomTo(1.f/2, pixelx , pixely);
			m_map->update();

		break;

		case KEY_EQUALS:
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
	}

}

void MapEditor::handleTool()
{
	
	const Rect & tileSrc = m_tilepicker->selection();
	const int tileId = m_tilepicker->selectionIndex();
	//if pixel tool
	switch(m_tool)
	{
		case MAP_TOOL_PIXEL:
			if (( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_DOWN )
				|| ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD ))
			{
				const Rect& cursor = m_map->tile( m_tooldata.mx, m_tooldata.my );
				if ( cursor.w != -1 )
				{
					//set tile
					int tilex, tiley;
					if ( m_map->getTileXY( cursor.x, cursor.y, tilex, tiley ) )
					{
						m_map->tiles[tiley * m_map->w + tilex] = tileId;
						m_map->update( { tilex, tiley, 1, 1 } );
					}
				}
			}
		break;
		case MAP_TOOL_ERASE:
			if (( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_DOWN )
				|| ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD ))
			{
				const Rect& cursor = m_map->tile( m_tooldata.mx, m_tooldata.my );
				if ( cursor.w != -1 )
				{
					//set tile
					int tilex, tiley;
					if ( m_map->getTileXY( cursor.x, cursor.y, tilex, tiley ) )
					{
						m_map->tiles[tiley * m_map->w + tilex] = TILE_CLEAR;
						m_map->update( { tilex, tiley, 1, 1 } );
					}
				}
			}
		break;

	}
}

void MapEditor::drawOverlay()
{
	//if no tileset
	if ( !m_map || !m_overlay || !m_tilepicker->tileset()) return;

	//draw overlaw
	//tile in map texture		
	memset(m_overlay->pixels, 0,  m_overlay->w * m_overlay->h * sizeof(Color));
	m_overlay->update();

	const Rect& tile = m_map->tile( m_tooldata.mx, m_tooldata.my );
	switch(m_tool)
	{
		case MAP_TOOL_PIXEL:
			if(tile.w != -1)
			{	
				const Rect & src = m_tilepicker->selection();
				const int texture = m_tilepicker->tileset()->texture;
				//Engine::Blit(texture, m_overlay->texture, src, tile);
				//Engine::DrawTextureRect( m_overlay->texture, BORDER_COLOR, tile, false);
				Engine::DrawTexture(texture, src, tile);
				Engine::DrawRect(BORDER_COLOR, tile, false);
				const int tx = tile.x * m_map->scale();
				const int ty = tile.y * m_map->scale();
				const int tw = tile.w * m_map->scale();
				const int th = tile.h * m_map->scale();

				m_overlay->update(tile);
			}	
		break;
		case MAP_TOOL_ERASE:
		{
			if(tile.w != -1)
			{	
				const Rect & src = m_tilepicker->selection();
				const int texture = m_tilepicker->tileset()->texture;
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
	//tileset must exist
	m_tilepicker->reload(tileset);
	m_map->tilesets[index] = tileset;
	m_map->reload();

}
