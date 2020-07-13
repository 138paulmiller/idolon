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
	m_context.create(VIEW_SHEET_EDITOR, m_sheetEditor = new SheetEditor());
}

void Editor::onExit() 
{

	printf("Exited Editor ");
}

void Editor::onTick() 
{
	signal(m_context.run());
}

void Editor::onKey(Key key, bool isDown) 
{
	m_context.handleKey(key,  isDown);
}


void Editor::editSheet(const std::string & sheetname)
{
	m_sheetEditor->setSheet(sheetname);
	m_context.enter(VIEW_SHEET_EDITOR);	
}