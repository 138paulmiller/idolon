#pragma once
/*
	This is the primary interace to the system. All apps should 

*/
#include "fs.hpp"
#include "shell.hpp"
#include "engine.hpp"
#include "editor.hpp"
#include "algo.hpp"
#include "assets.hpp"
#include "commands.hpp"
#include "graphics.hpp"

enum : uint8
{
	APP_SHELL = 0,
	APP_TILESET_EDITOR,
	APP_MAP_EDITOR,
	APP_COUNT
};

namespace Sys
{
	void Startup( const CommandTable & cmds);
	void Shutdown();
	int Run();
	const std::string& Path();
	const std::string& AssetPath();
	Shell* GetShell();
	Context* GetContext();
}