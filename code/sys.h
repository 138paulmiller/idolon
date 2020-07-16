#pragma once
/*
	This is the primary interace to the system. All apps should 

*/
#include "fs.h"
#include "shell.h"
#include "engine.h"
#include "graphics.h"
#include "editor.h"
#include "assets.h"
#include "tools.h"

enum : uint8_t
{
	APP_SHELL = 0,
	APP_SHEET_EDITOR,
	APP_COUNT
};

namespace Sys
{
	void Startup( const CommandTable & cmds);
	void Shutdown();
	int Run();
	const std::string& AssetPath();
	Shell* GetShell();
	Editor* GetEditor();
	UI::Toolbar* GetMenu();
	Context* GetContext();
}