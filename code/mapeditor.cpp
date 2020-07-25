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
	for(int y = TILE_H; y < h; y+=SPRITE_H)
		for(int x = TILE_W; x < w; x+=SPRITE_W)
		{	
			Sprite * sprite = new Sprite( 1, TILE_W, TILE_H );
			sprite->sheet = m_mapName;
			sprite->x = x;
			sprite->y = y;
			sprite->reload();
			m_sprites.push_back(sprite);
		}
	LOG( "\nLoaded %u\n", m_sprites.size() );
	m_map = Assets::Load<Map>(m_mapName);
	LOG( "\nFound map %d\n", m_map);
	
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
	if(m_map)
		m_map->draw();

	for(Sprite * sprite : m_sprites )
		sprite->draw();
	
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
		}
		LOG( "\nTile Index %d\n", tile );
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