
#include "../sys.hpp"
#include "gameeditor.hpp"
#include <iostream>


GameEditor::GameEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{}

void GameEditor::onEnter()
{

	LOG("Entering game editor ... ");
	int w, h;
	Engine::GetSize(w, h);

	//App::addWidget( m_toolbar );

	Editor::onEnter();

}

void GameEditor::onExit()
{
	App::clear();
	Editor::onExit();
	LOG("Exited game editor");
}



void GameEditor::onTick()
{
	Engine::ClearScreen(EDITOR_COLOR);
}

//
void GameEditor::onKey(Key key, bool isDown)
{
}

void GameEditor::undo()
{ 	
}

void GameEditor::redo()
{

}

void GameEditor::save()
{
}

void GameEditor::load( const std::string & gamepath )
{
	std::string m_gamepath;
}