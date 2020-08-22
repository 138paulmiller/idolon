#include "ui.hpp"
#include "editor.hpp"

//----------------------------------------------------------------------------

Editor::Editor(uint8 support) 
		:m_support(support)
{
	m_menu = -1;
}

void Editor::redo()  
{
	LOG( "Editor: Redo unimplemented" );
}

void Editor::undo()  
{

	LOG( "Editor: Undo unimplemented" );
}

void Editor::save()  
{
	LOG( "Editor: Save unimplemented" );
}


bool Editor::supports(AppSupport support)
{
	return (m_support >> support) & 1U; 
}

void Editor::onExit()
{
	if(m_menu != -1) 
		App::removeWidget(m_menu);
	if ( m_toolbar != -1 )
		App::removeWidget( m_toolbar );
}

void Editor::onEnter()
{
	if ( m_menu != -1 )
		App::removeWidget( m_menu );

	const std::string &fontName = "full";

	int screenw, screenh;
	UI::Toolbar *toolbar = new UI::Toolbar( this, 0, 0 );
	toolbar->font = fontName;
	m_toolbar = App::addWidget( toolbar );

	int screenW, screenH;
	Engine::GetSize( screenW, screenH );
	UI::Toolbar *menu = new UI::Toolbar( this, screenW, 0 );
	m_menu = App::addWidget(menu);

	menu->font = fontName; //has glyphs
	menu->leftAlign = false;
	
	//BACK
	menu->add({'X'}, [&](){
		App::signal(APP_CODE_EXIT);
	}, false);
	
	//get tool bar offset
	m_menuY = menu->get( 0 )->rect().h + menu->get( 0 )->rect().y;

	//SAVE
	if(supports(APP_SAVE))
	{	
		menu->add({20}, [&](){
			this->save();
		}, false);
	}
	//REDO
	if(supports(APP_REDO))
	{
		menu->add({19}, [&](){
			this->redo();
		}, false);
	}
	//UNDO
	if(supports(APP_UNDO))
	{	
		menu->add({18}, [&](){
			this->undo();
		}, false);
	}
}

void Editor::hideTools( bool hidden )
{
	UI::Toolbar * toolbar = dynamic_cast<UI::Toolbar*>(App::getWidget( m_toolbar ));
	toolbar->hidden = hidden;
}

void Editor::addTool( const std::string &text, std::function<void()> click, bool sticky )
{
	UI::Toolbar * toolbar = dynamic_cast<UI::Toolbar*>(App::getWidget( m_toolbar ));
	toolbar->add( text, click, sticky );	
	toolbar->get( 0 )->cbClick();
	
}