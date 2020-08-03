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


	m_tilepicker = new UI::TilePicker();
	m_map = Assets::Load<Graphics::Map>(m_mapName);
	//TODO - create combox box to select tilesets  
	setTileset( m_mapName );	//by default


	
	const int toolY = h - (m_tilepicker->rect().h + charH);
	const int maph = toolY - TILE_H;

	m_map->rect = { 0, TILE_H, w, maph };
	//reset view
   	m_map->zoomTo(1.0, 0, 0 );

	//TODO - text edit to set tileset
	m_tooldata = {0,0, m_map->tilew, m_map->tileh};
	m_tool = MAP_TOOL_PIXEL;


	m_toolbar = new UI::Toolbar(this, 0, toolY);

	m_toolbar->add("PIXEL", [&](){
		m_tool = MAP_TOOL_PIXEL;                     
	});
	
	m_toolbar->add("FILL", [&](){
		m_tool = MAP_TOOL_FILL;             
	});

	m_toolbar->add("LINE", [&](){
		m_tool = MAP_TOOL_LINE;
		m_tooldata = { -1, -1, -1, -1 };
	});

	m_toolbar->add("ERASE", [&](){
		m_tool = MAP_TOOL_ERASE;
		m_tooldata = { -1, -1, 1, 1 };

	});

	//first add toolbat	
	//add the ui widgets
	App::addWidget( m_toolbar );

	App::addWidget(m_tilepicker);
	Editor::onEnter();


/////////////// DEbug //////////////////////////
	const int spriteId =8 ;
	for(int y = SPRITE_H; y < m_map->rect.h; y+=SPRITE_H*2)
	{

		for(int x = SPRITE_W; x < w; x+=SPRITE_W*2)
		{	
			Graphics::Sprite * sprite = new Graphics::Sprite( spriteId );
			sprite->tileset = m_map->tileset;
			sprite->x = x;
			sprite->y = y;
			sprite->reload();
			m_sprites.push_back(sprite);
		}
	}
///////////////////////////////////////////////////
}

void MapEditor::onExit()
{	
	
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

	//draw tool
	switch(m_tool)
	{
		case MAP_TOOL_PIXEL:
		{	
			const Rect& cursor = m_map->tile( m_tooldata.mx, m_tooldata.my );
			if(cursor.w != -1)
			{	
				const Rect & src = m_tilepicker->selection();
				const int texture = m_tilepicker->tileset()->texture;
				Engine::DrawTexture(texture, src, cursor);
				Engine::DrawRect(BORDER_COLOR, cursor, false);
			}	
		}
		break;
	}
	

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

void MapEditor::handleTool()
{
	
	const Rect & tileSrc = m_tilepicker->selection();
	const int tileId = m_tilepicker->selectionIndex();
	//if pixel tool
	switch(m_tool)
	{
		case MAP_TOOL_PIXEL:
		{	
			if (( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_DOWN )
				|| ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD ))
			{

				const Rect& cursor = m_map->tile( m_tooldata.mx, m_tooldata.my );
				if ( cursor.w != -1 )
				{
					//set tile
					int tilex, tiley;
					m_map->getTileXY( cursor.x, cursor.y, tilex, tiley );
					m_map->tiles[tiley * m_map->w + tilex] = tileId;
					m_map->update( { tilex, tiley, 1, 1 } );
				}
			}
		}
		break;
	}
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

	int tile = 0;
	printf("Key %d Alt : %d\n", key, KEY_ALT);
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

	case KEY_x:
		//zoom in
		m_map->zoomTo(1.f/2, pixelx , pixely);
		m_map->update();

	break;

	case KEY_z:
		//zoom in
		m_map->zoomTo(2.f, pixelx, pixely);
		m_map->update();
	break;
	}

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

void MapEditor::setTileset( const std::string& tileset )
{

	m_tilepicker->reload(tileset);
	m_map->tileset = tileset;
	m_map->reload();
}
