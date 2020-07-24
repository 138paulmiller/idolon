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
	printf("Entering map editor ... \n");
	int w,h;
	Engine::GetSize(w,h);
	//load empty	tilesets.
	//TODO load these from sprite assets	
	for(int y = TILE_H_SMALL; y < h; y+=TILE_H_SMALL)
		for(int x = TILE_W_SMALL; x < w; x+=TILE_W_SMALL)
		{	
			SpriteInstance * sprite = new SpriteInstance( m_mapName );
			sprite->rect.x = x;
			sprite->rect.y = y;
			m_sprites.push_back(sprite);
		}

	Editor::onEnter();
}

void MapEditor::onExit()
{	
	for(SpriteInstance * sprite : m_sprites )
		delete sprite;
	m_sprites.clear();
	App::clear();
	Editor::onExit();
	printf("Extited map editor\n");
}



void MapEditor::onTick()
{
	Engine::ClearScreen(EDITOR_COLOR);
	for(SpriteInstance * sprite : m_sprites )
		sprite->draw();
}


//
void MapEditor::onKey(Key key, bool isDown)
{

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