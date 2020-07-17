#include "pch.hpp"

#include "context.hpp"

Context::Context( uint8_t appCount )
{
	m_appCount = appCount;
	m_apps = new UI::App * [appCount];
	memset( m_apps, 0, appCount * sizeof( UI::App *) );
	m_prevAppId =  0;
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
	for ( uint8_t i = 0; i < m_appCount; i++ )
	{
		delete m_apps[i];
		m_apps[i] = 0;
	}
}

void Context::create( uint8_t appId, UI::App * app )
{
	ASSERT(appId < m_appCount, "Invalid App ID");
	m_apps[appId] = app;
}

void Context::enter(uint8_t appId)
{
	ASSERT(appId < m_appCount, "Invalid App ID");
	printf("Context Switch: from %d to %d\n", m_prevAppId, appId);
	if(m_app)
		m_app->onExit();

	m_prevAppId = m_appId;
	m_app = m_apps[m_appId = appId];
	if(m_app)
		m_app->onEnter();
}
void Context::exit()
{
	printf("Exiting Context...\n");

	enter(m_prevAppId);
}
void Context::handleKey( Key key, bool isDown )
{
	if(m_app)
		m_app->onKey(key, isDown);
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
