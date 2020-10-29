
#include "app.hpp"
#include "ui.hpp"
#include <cstring>

Context::Context( uint8 appCount )
{
	m_appCount = appCount;
	m_apps = new App * [appCount];
	memset( m_apps, 0, appCount * sizeof( App *) );
	m_appId  = 0;
	m_app = 0;
}
Context::~Context()
{
	clear();
	delete[] m_apps;
}
void Context::clear( )
{
	if ( m_app )
		m_app->onExit( );
	m_app = 0;
	for ( uint8 i = 0; i < m_appCount; i++ )
	{
		delete m_apps[i];
		m_apps[i] = 0;
	}
}

void Context::create( uint8 appId, App * app )
{
	ASSERT(appId < m_appCount, "Invalid App ID");
	m_apps[appId] = app;
}

void Context::enter(uint8 appId, bool kill )
{
	//already withiin
	ASSERT(appId < m_appCount, "Invalid App ID");

	const int prevAppId = m_appStack.empty() ? -1 : m_appStack.back();
	if ( prevAppId != -1 )
	{
		LOG("Context Switch: from %d to %d\n", prevAppId, appId);
	}

	if ( m_app )
	{
		if ( kill )
		{
			m_app->signal( APP_CODE_EXIT ); 
			m_app->onExit();
			m_appStack.pop_back();
		}
	}
	//if not dead, can reenter
	if ( !kill )
	{
		m_appStack.push_back( m_appId );
	}
	m_app = m_apps[m_appId = appId];

	if ( m_app  && m_app->status() != APP_CODE_CONTINUE )
	{
		m_app->signal( APP_CODE_CONTINUE ); //allow reenter
		m_app->onEnter();
	}
	else
	{
		LOG("Entering empty app ...\n");
	}
}
void Context::exit()
{
	LOG("Exiting Context...\n");

	const int prevAppId = m_appStack.empty() ? -1 : m_appStack.back();
	if ( prevAppId != -1 )
	{
		enter(prevAppId, true );
	}
}
void Context::handleKey( Key key, ButtonState state)
{
	if(m_app)
		m_app->onKey(key, state);
}

AppCode Context::run( )
{
	if(!m_app) return APP_CODE_EXIT;
	
	m_app->update();
	//
	m_app->onTick();
	//draw ui layer on top
	m_app->draw();

	return m_app->status();
}
	//////////////////////////////////////////////////////////////////////////////////
	
App::App()
{
	m_status = APP_CODE_EXIT;
}

App::~App()
{
	clear();
}

void App::clear()
{

	for(UI::Widget * widget : m_widgets)
	{
		if(widget)
			delete widget;
	}
	for(UI::Button * button : m_buttons)
	{
		if(button)
			delete button;
	}
	m_widgets.clear();
	m_buttons.clear();
}

void App::signal(AppCode code)
{
	m_status = code;
}
	
AppCode App::status()
{
	return m_status ;
}
	
void App::update()
{

	for ( UI::Widget *widget : m_widgets )
	{
		if ( widget )
		{
			widget->onUpdate();
		}
	}
	int mx, my;
	Engine::GetMousePosition(mx, my);
	for(UI::Button * button : m_buttons)
	{
		if(button)
		{
			button->onReset();
				
			//handle collision only for visible buttons
			if ( !button->hidden )
			{
				if(button->rect().intersects({mx, my, 1,1}))
				{
					if( Engine::GetMouseButtonState(MouseButton::MOUSEBUTTON_LEFT) == BUTTON_CLICK)
					{
						button->click();
					}							
					else if( Engine::GetMouseButtonState(MouseButton::MOUSEBUTTON_LEFT) != BUTTON_HOLD)
					{
						button->leave();

						if(button->cbHover)
							button->cbHover();
						button->onHover();
					}
				}
				else
				{
					if ( Engine::GetMouseButtonState( MouseButton::MOUSEBUTTON_LEFT ) == BUTTON_CLICK )
					{
						button->leave();
					}
				}
			}
			button->onUpdate();
		}
	}
}


void App::draw()
{	
	for(UI::Widget * widget : m_widgets)
	{
		if(widget && !widget->hidden)
		{
			widget->onDraw();
		}
	}

	for(UI::Button * button : m_buttons)
	{
		if(button && !button->hidden)
		{
			button->onDraw();
		}	
	}
}

int App::addWidget(UI::Widget * widget)
{
	int idx = m_widgets.size();
	m_widgets.push_back(widget);
	return idx;
}

int App::addButton(UI::Button * button)
{

	int idx = m_buttons.size();
	m_buttons.push_back(button);
	return idx;
}
UI::Button * App::getButton(int idx)
{
	if(idx < m_buttons.size())
		return m_buttons[idx];
	return 0;
}

UI::Widget * App::getWidget(int idx)
{
	if(idx < m_widgets.size())
		return m_widgets[idx];
	return 0;
}
int App::getButtonCount()
{
	return m_buttons.size();
}
	
int App::getWidgetCount()
{
	return m_widgets.size();
}

void App::removeButton(int idx)
{

	if(idx >= m_buttons.size()) return;
	delete m_buttons[idx];
	m_buttons[idx] = 0;
}

void App::removeWidget(int idx)
{
	if(idx >= m_widgets.size()) return;
	delete m_widgets[idx];
	m_widgets[idx] = 0;

}
	