#pragma once
#include "pch.h"

//Create terminal system. Echos key input to a buffer. has line offset nd render the text to the screen

namespace Editor
{
	void Startup();
	void Shutdown();
	//name of sheet to edit
	int RunSheetView(const std::string & name);
}