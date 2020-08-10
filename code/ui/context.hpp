#pragma once 

#include "ui.hpp"

//App context
class Context
{
public:
	Context( uint8 appCount );
	~Context();
	void clear();
	void create( uint8 appId, UI::App * app );
	//kill previous process
	void enter(uint8 appId, bool kill = true);
	void exit();
	void handleKey( Key key, bool isDown );
	AppCode run( );



	template <typename Type= UI::App>
	Type * app()
	{
		return dynamic_cast<Type*>(m_app);
	}

	template <typename Type>
	Type * app(int appId)
	{
		return dynamic_cast<Type*>(m_apps[appId]);
	}

private:

	uint8 m_appId;
	uint8 m_prevAppId;
	UI::App * m_app;	

	uint8 m_appCount;
	UI::App ** m_apps = { 0} ;
};