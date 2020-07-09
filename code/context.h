#pragma once 

#include "ui.h"

//App context
class Context
{
public:
	Context( uint8_t appCount );
	~Context();
	void create( uint8_t appId, UI::App * app );
	void enter(uint8_t appId);
	void exit();
	void handleKey( Key key, bool isDown );
	void run( );

	template <typename Type>
	Type * app()
	{
		return dynamic_cast<Type*>(g_app);
	}

	template <typename Type>
	Type * app(int appId)
	{
		return dynamic_cast<Type*>(g_apps[appId]);
	}

private:

	uint8_t g_appId;
	uint8_t g_prevAppId;
	UI::App * g_app;	

	uint8_t g_appCount;
	UI::App ** g_apps = { 0} ;
};