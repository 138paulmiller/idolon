#pragma once 

#include "ui.h"

//App context
namespace Context
{
	void Startup( uint8_t appCount );
	void Shutdown();
	void Register( uint8_t appId, UI::App * app );
	void Enter(uint8_t appId);
	void Exit();
	void HandleKey( Key key, bool isDown );
	void Run( );
}