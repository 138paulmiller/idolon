#include "ui.hpp"
#include "editor.hpp"

//----------------------------------------------------------------------------

Editor::Editor(uint8 support) 
		:m_support(support)
{
	m_control = -1;
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
	if(m_control != -1) 
		App::removeWidget(m_control);
	if ( m_toolbar != -1 )
		App::removeWidget( m_toolbar );
}

void Editor::onEnter()
{
	if ( m_control!= -1 )
		App::removeWidget( m_control );

	const std::string &fontName = "full";

	int screenw, screenh;
	UI::Toolbar *toolbar = new UI::Toolbar( this, 0, 0 );
	toolbar->font = fontName;
	m_toolbar = App::addWidget( toolbar );

	int screenW, screenH;
	Engine::GetSize( screenW, screenH );
	UI::Toolbar *control = new UI::Toolbar( this, screenW, 0 );
	m_control = App::addWidget(control);

	control->font = fontName; //has glyphs
	control->leftAlign = false;
	
	//BACK
	control->add({'X'}, [&](){
		App::signal(APP_CODE_EXIT);
	}, false);
	
	//get tool bar offset
	m_controlX = control->get( 0 )->rect().x;
	m_controlY = control->get( 0 )->rect().h + control->get( 0 )->rect().y;

	//SAVE
	if(supports(APP_SAVE))
	{	
		control->add({20}, [&](){
			this->save();
		}, false);
	}
	//REDO
	if(supports(APP_REDO))
	{
		control->add({19}, [&](){
			this->redo();
		}, false);
	}
	//UNDO
	if(supports(APP_UNDO))
	{	
		control->add({18}, [&](){
			this->undo();
		}, false);
	}
}

void Editor::hideControl( bool hidden )
{
	UI::Toolbar * control = dynamic_cast<UI::Toolbar*>(App::getWidget( m_control));
	control->hidden = hidden;
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