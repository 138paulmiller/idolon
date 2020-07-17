#pragma once
/*
	This is the primary interace to the system. All apps should 

*/
#include "fs.hpp"
#include "tools.hpp"
#include "shell.hpp"
#include "engine.hpp"
#include "editor.hpp"
#include "assets.hpp"
#include "graphics.hpp"

enum : uint8
{
	APP_SHELL = 0,
	APP_TILESET_EDITOR,
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
	Context* GetContext();
}