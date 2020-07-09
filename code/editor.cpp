#pragma once

#include "editor.h"
#include "shell.h"
#include "sheeteditor.h"

Toolbar::Toolbar()
{

}

//----------------------------------------------------------------------------

Editor::Editor()
{

}

void Editor::onEnter()
{
	m_views[VIEW_SHEET_EDITOR] = new SheetEditor();
	m_view = 0;
}

void Editor::onExit() 
{
	if(	m_view)
		m_view->onExit();
	for(int i=0; i < VIEW_COUNT; i++)
		delete m_views[i];
}

void Editor::onTick() 
{
	if(!m_view) return;
	m_view->update( );
	m_view->onTick( );
	m_view->draw( );
}

void Editor::onKey(Key key, bool isDown) 
{
	if(m_view)
		m_view->onKey( key, isDown);
}
void Editor::switchView(int viewId)
{
	if(m_view)
		m_view->onExit();

	m_prevViewId = m_viewId;
	m_viewId = viewId;
	m_view = m_views[m_viewId ];

	if(m_view)
		m_view->onEnter();

}


void Editor::editSheet(const std::string & sheetname)
{
	getView<SheetEditor>(VIEW_SHEET_EDITOR)->setSheet(sheetname);
	switchView(VIEW_SHEET_EDITOR);	
}