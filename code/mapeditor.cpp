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
	for(int y = TILE_H_SMALL; y < h; y+=TILE_H)
		for(int x = TILE_W_SMALL; x < w; x+=TILE_W)
		{	
			Sprite * sprite = new Sprite( 22, TILE_W_SMALL, TILE_H_SMALL );
			sprite->sheet = m_mapName;
			sprite->x = x;
			sprite->y = y;
			sprite->reload();
			m_sprites.push_back(sprite);
		}
	LOG( "\nLoaded %u\n", m_sprites.size() );

	Editor::onEnter();
}

void MapEditor::onExit()
{	
	for(Sprite * sprite : m_sprites )
		delete sprite;
	m_sprites.clear();
	App::clear();
	Editor::onExit();
	LOG("Extited map editor\n");
}



void MapEditor::onTick()
{
	Engine::ClearScreen(EDITOR_COLOR);
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
}

void MapEditor::setMap( const std::string& name )
{
	m_mapName = name;
}