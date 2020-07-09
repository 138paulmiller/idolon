#pragma once 

#include "context.h"


Context::Context( uint8_t appCount )
{
	g_appCount = appCount;
	g_apps = new UI::App * [appCount];
	g_prevAppId =  0;
	g_appId  = 0;
}
Context::~Context()
{
	if(	g_app)
		g_app->onExit();
	for(uint8_t i=0; i < g_appCount; i++)
		delete g_apps[i];
	delete g_apps;
}
void Context::create( uint8_t appId, UI::App * app )
{
	ASSERT(appId < g_appCount);
	g_apps[appId] = app;
}

void Context::enter(uint8_t appId)
{
	ASSERT(appId < g_appCount);
	if(g_app)
	g_app->onExit();

	g_prevAppId = g_appId;
	g_app = g_apps[g_appId = appId];
	if(g_app)
		g_app->onEnter();
}
void Context::exit()
{
	enter(g_prevAppId);
}
void Context::handleKey( Key key, bool isDown )
{
	if(g_app)
		g_app->onKey(key, isDown);
}

void Context::run( )
{
	if(!g_app) return;
	g_app->update();
	//
	g_app->onTick();
	//draw ui layer on top
	g_app->draw();
}
