#include "ui.hpp"
#include "editor.hpp"

//----------------------------------------------------------------------------


namespace
{


}

Editor::Editor(uint8 support) 
		:m_support(support)
{
	m_control = -1;
}

bool Editor::supports(AppSupport support)
{
	return (m_support >> support) & 1U; 
}

void Editor::onExit()
{
	App::clear();
}

void Editor::onEnter()
{
	if ( m_control!= -1 )
		App::removeWidget( m_control );

	const std::string &fontName = DEFAULT_FONT_ICONS;
	Graphics::Font * font = Assets::Load<Graphics::Font>(fontName);
	const int charW = font->charW;
	const int charH = font->charH;
	Assets::Unload<Graphics::Font>(fontName);

	int screenw, screenh;
	UI::Toolbar *toolbar = new UI::Toolbar( this, 0,  charH );
	toolbar->font = fontName;
	m_toolbar = App::addWidget( toolbar );


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
	toolbar->add( TranslateIcon(icon), click, sticky, false );		
	if ( sticky )
	{
		toolbar->get( 0 )->click();
	}
}

