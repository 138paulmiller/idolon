#pragma once
#include <string>

//Create terminal system. Echos key input to a buffer. has line offset nd render the text to the screen

namespace Editor
{
	void Startup();
	void Shutdown();
	//TODO Create universal toolbar that runs when editor is running
	int RunSheetView(const std::string & name);
}