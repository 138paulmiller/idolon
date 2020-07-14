#pragma once

#include "editor.h"
#include "shell.h"
#include "sheeteditor.h"

Toolbar::Toolbar()
{

}

//----------------------------------------------------------------------------

Editor::Editor() 
	: m_context(VIEW_COUNT)
{

}

void Editor::onEnter()
{

	printf("Entering Editor ...\n");
	m_sheetEditor = new SheetEditor(  );
	m_context.create(VIEW_SHEET_EDITOR, m_sheetEditor );
}

void Editor::onExit() 
{
	printf("Exited Editor ");
	App::clear();
}

void Editor::onTick() 
{
	if(App::status() != APP_CODE_EXIT)
		App::signal(m_context.run());
}

void Editor::onKey(Key key, bool isDown) 
{
	m_context.handleKey(key,  isDown);
}

void Editor::switchView()
{	
	App::clear();
	App::addWidget(m_menu = new UI::Toolbar(this, 0, 0));

	m_menu->add("BACK", [&](){
		App::signal(APP_CODE_EXIT);
	}, false);
	
	if(m_context.app()->supports(APP_SAVE))
	{	
		m_menu->add("SAVE", [&](){
			m_context.app()->save();
		}, false);
	}

	if(m_context.app()->supports(APP_UNDO))
	{	
		m_menu->add("UNDO", [&](){
			m_context.app()->undo();
		}, false);
	}
	if(m_context.app()->supports(APP_REDO))
	{
		m_menu->add("REDO", [&](){
			m_context.app()->redo();
		}, false);
	}
}	

void Editor::editSheet(const std::string & sheetname)
{
	m_sheetEditor->setSheet(sheetname);
	m_context.enter(VIEW_SHEET_EDITOR);	

	switchView();
}