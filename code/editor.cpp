
#include "pch.h"
#include "editor.h"
#include "shell.h"
#include "sheeteditor.h"

//----------------------------------------------------------------------------

Editor::Editor(uint8_t support) 
		:m_support(support)
{
	m_menu = -1;
}
void Editor::redo()  
{
	printf( "Editor: Redo unimplemented" );
}

void Editor::undo()  
{

	printf( "Editor: Undo unimplemented" );
}

void Editor::save()  
{
	printf( "Editor: Save unimplemented" );
}


bool Editor::supports(AppSupport support)
{
	return (m_support >> support) & 1U; 
}
void Editor::onExit()
{
	if(m_menu != -1) 
		App::removeWidget(m_menu);
}

void Editor::onEnter()
{
	if(m_menu != -1) 
		App::removeWidget(m_menu);
	
	UI::Toolbar * menu = new UI::Toolbar(this, 0, 0);
	m_menu = App::addWidget(menu);
	
	//BACK
	menu->add({27}, [&](){
		App::signal(APP_CODE_EXIT);
	}, false);
	//SAVE
	if(supports(APP_SAVE))
	{	
		menu->add({25}, [&](){
			this->save();
		}, false);
	}
	//UNDO
	if(supports(APP_UNDO))
	{	
		menu->add({17}, [&](){
			this->undo();
		}, false);
	}
	//REDO
	if(supports(APP_UNDO))
	{
		menu->add({16}, [&](){
			this->redo();
		}, false);
	}
}