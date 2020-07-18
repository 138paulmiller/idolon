
#include "pch.hpp"

#include "shell.hpp"
#include "editor.hpp"

//----------------------------------------------------------------------------

Editor::Editor(uint8 support) 
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
	menu->font = "full"; //has glyphs
	
	m_menu = App::addWidget(menu);
	//BACK
	menu->add({27}, [&](){
		App::signal(APP_CODE_EXIT);
	}, false);
	//SAVE
	if(supports(APP_SAVE))
	{	
		menu->add({20}, [&](){
			this->save();
		}, false);
	}
	//UNDO
	if(supports(APP_UNDO))
	{	
		menu->add({18}, [&](){
			this->undo();
		}, false);
	}
	//REDO
	if(supports(APP_UNDO))
	{
		menu->add({19}, [&](){
			this->redo();
		}, false);
	}
}