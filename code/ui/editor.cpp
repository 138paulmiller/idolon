#include "ui.hpp"
#include "editor.hpp"

//----------------------------------------------------------------------------


namespace
{
	//TODO move this table to ini file!!
	//map from icon name to tileindex repr
	std::unordered_map<std::string, std::string> s_iconNameMap = {
		{ "PENCIL", { 0  } },
		{ "FILL",   { 1  } },
		{ "LINE",   { 2  } },
		{ "ERASER", { 3  } },
		{ "UNDO",   { 4  } },
		{ "REDO",   { 5  } },
		{ "RELOAD", { 6  } },
		{ "SAVE",   { 7  } },
		{ "LOAD",   { 8  } },
		{ "EXIT",   { 9  } },
		{ "STAMP",  { 10 } },
		{ "BUILD",  { 11 } },
		{ "PLAY",   { 12 } },
		{ "PAUSE",  { 13 } },
		{ "DOWN",   { 14 } },
		{ "UP",     { 15 } },
	};

}

Editor::Editor(uint8 support) 
		:m_support(support)
{
	m_control = -1;
}


std::string Editor::getIcon( const std::string &name )
{
	auto iconIt = s_iconNameMap.find( name );
	if ( iconIt != s_iconNameMap.end() )
	{
		return iconIt->second;
	}
	//assert!
	return "";
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

	const std::string &fontName = DEFAULT_FONT_ICONS;

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
	control->add( getIcon("EXIT"), [&](){
		App::signal(APP_CODE_EXIT);
	}, false, false );
	
	//get tool bar offset
	m_controlX = control->get( 0 )->rect().x;
	m_controlY = control->get( 0 )->rect().h + control->get( 0 )->rect().y;

	//SAVE
	if(supports(APP_SAVE))
	{	
		control->add( getIcon("SAVE"), [&](){
			this->save();
		}, false, false);
	}
	//REDO
	if(supports(APP_REDO))
	{
		control->add( getIcon("REDO"), [&](){
			this->redo();
		}, false, false);
	}
	//UNDO
	if(supports(APP_UNDO))
	{	
		control->add( getIcon("UNDO"), [&](){
			this->undo();
		}, false, false);
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

void Editor::addTool( const std::string &icon, std::function<void()> click, bool sticky )
{
	UI::Toolbar * toolbar = dynamic_cast<UI::Toolbar*>(App::getWidget( m_toolbar ));
	toolbar->add( getIcon(icon), click, sticky, false );	
	toolbar->get( 0 )->cbClick();
	
}