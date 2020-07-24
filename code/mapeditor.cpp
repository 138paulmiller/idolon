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
	printf("Entering map editor ... ");
	//load empty	tilesets.
	//TODO load these from sprite assets	
	m_sprite = new Sprite( TILE_W_SMALL, TILE_H_SMALL, "Sprite_test" );
	m_sprite->sheet = m_mapName ;
	m_sprite->rect.x = 16;
	m_sprite->rect.y = 16;

	m_sprite->reload();

	Editor::onEnter();
}

void MapEditor::onExit()
{	
	m_tileset = Assets::Load<Graphics::Tileset>(m_mapName );
	m_mapName = "";

	App::clear();
	Editor::onExit();
	printf("Extited map editor");
}



void MapEditor::onTick()
{
	Engine::ClearScreen(EDITOR_COLOR);
	m_sprite->draw();
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