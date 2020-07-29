#include "pch.hpp"
#include "mapeditor.hpp"
#include <iostream>


using namespace Graphics;
using namespace UI;

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
{}

void MapEditor::onEnter()
{
	LOG("Entering map editor ... \n");
	int w,h;
	Engine::GetSize(w,h);
	//load empty	tilesets.
	//TODO load these from sprite assets	

//	m_map->zoom(zoom);
	m_map = Assets::Load<Map>(m_mapName);
	LOG( "\nLoaded %u\n", m_sprites.size() );

	int tile =8 ;
	for(int y = SPRITE_H; y < h; y+=SPRITE_H*2)
		for(int x = SPRITE_W; x < w; x+=SPRITE_W*2)
		{	
			Sprite * sprite = new Sprite( 8 );
			sprite->tileset = m_map->sheet;
			sprite->x = x;
			sprite->y = y;
			sprite->reload();
			m_sprites.push_back(sprite);
		}


	//TODO - text edit to set tileset

	Editor::onEnter();
}

void MapEditor::onExit()
{	
	for(Sprite * sprite : m_sprites )
		delete sprite;
	m_sprites.clear();
	Assets::Unload<Map>(m_mapName);

	App::clear();
	Editor::onExit();
	LOG("Extited map editor\n");
}



void MapEditor::onTick()
{
	const float scale  =m_map->scale();
	//Engine::SetDrawBlendMode(BLEND_MULTIPLY);
	const Rect& view = m_map->getView();
	int mx, my;
	Engine::GetMousePosition(mx, my);


	MouseButton btn = m_shift ? MOUSEBUTTON_LEFT : MOUSEBUTTON_RIGHT;

	//scroll if necessary
	if ( Engine::GetMouseButtonState( btn ) == BUTTON_DOWN )
	{
		m_prevmx = mx;
		m_prevmy = my;
	}
	else if ( Engine::GetMouseButtonState( btn ) == BUTTON_HOLD )
	{
		const int dx = m_prevmx - mx;
		const int dy = m_prevmy - my;
		m_map->scroll(dx, dy);
		m_prevmx = mx;
		m_prevmy = my;

	}

	Engine::ClearScreen(EDITOR_COLOR);
	if(m_map)
	{
		m_map->draw();
	}

	for(Sprite * sprite : m_sprites )	
	{
		sprite->draw();
	}	
	// Draw cursor
	const int tilew =  m_map->tilew/scale;
	const int tileh =  m_map->tileh/scale;

	const int tilex =  view.x/scale + mx;
	const int tiley =  view.y/scale + my;

	const int bx = (int)tilex/tilew*tilew - view.x/scale;
	const int by = (int)tiley/tileh*tileh - view.y/scale;
	const Rect& cursor = 
	{ 
		(int)(bx),
		(int)(by),
		(int)(tilew), 
		(int)(tileh) 
	};
	//move cursor to ui element ? 
	Engine::DrawRect(BORDER_COLOR, cursor, false);





}


//
void MapEditor::onKey( Key key, bool isDown )
{
	const Rect& view = m_map->getView();
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

	m_map->update();
	Assets::Save(m_map);
}

void MapEditor::setMap( const std::string& name )
{
	m_mapName = name;
}