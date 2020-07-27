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

	int tile =0 ;
	for(int y = SPRITE_H; y < h; y+=SPRITE_H*2)
		for(int x = SPRITE_W; x < w; x+=SPRITE_W*2)
		{	
			Sprite * sprite = new Sprite( tile, SPRITE_W, SPRITE_H );
			sprite->sheet = m_mapName;
			sprite->x = x * WINDOW_SCALE;
			sprite->y = y * WINDOW_SCALE;
			sprite->reload();
			m_sprites.push_back(sprite);
			if(++tile >= SPRITE_COUNT) tile=0;
		}

	LOG( "\nLoaded %u\n", m_sprites.size() );
	m_map = Assets::Load<Map>(m_mapName);
	
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
	Engine::ClearScreen(EDITOR_COLOR);
	
	if(m_map)m_map->draw();

	for(Sprite * sprite : m_sprites )	sprite->draw();
	
}


//
void MapEditor::onKey( Key key, bool isDown )
{
	int tile = 0;
	static bool released = true;
	if ( !isDown )
	{
		released = true;
		return;
	}
	if ( released )
	{
		released = false;
		switch ( key )
		{
			case KEY_z:
				for ( Sprite* sprite : m_sprites )
				{
					tile = sprite->tile--;
				}
			break;
		
			case KEY_x:
				for ( Sprite* sprite : m_sprites )
				{
					tile = sprite->tile++;
				}
			break;
			case KEY_d:
				printf("Modifying!!!\n");
				for(int y = 0; y < m_map->h; y++)
					for(int x = 0; x < m_map->w; x++)
					{
						const int i = y * m_map->w + x;
						m_map->tiles[i] = i % TILE_COUNT; //( ( x % 2 ) && ( y % 2 ) ) | ( !( x % 2 ) && ( y + 1 % 2 ) );
					}
				m_map->reload();
			break;
		}
		LOG( "\nTile Index %d\n", tile );
	}
	switch(key)
	{
	case KEY_UP:
		m_map->scroll( 0, -1 );
	break;

	case KEY_DOWN:
		m_map->scroll( 0, 1 );
	break;

	case KEY_LEFT:
		m_map->scroll( -1, 0 );
	break;

	case KEY_RIGHT:
		m_map->scroll( 1, 0 );
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