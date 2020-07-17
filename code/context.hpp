#pragma once 

#include "ui.hpp"

//App context
class Context
{
public:
	Context( uint8_t appCount );
	~Context();
	void clear();
	void create( uint8_t appId, UI::App * app );
	void enter(uint8_t appId);
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

	uint8_t m_appId;
	uint8_t m_prevAppId;
	UI::App * m_app;	

	uint8_t m_appCount;
	UI::App ** m_apps = { 0} ;
};