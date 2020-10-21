#pragma once
/*
	This is the primary interace to the system. All apps should 

*/
#include "config.hpp"
#include "game.hpp"
#include "algo.hpp"
#include "shell.hpp"
#include "ui/api.hpp"
#include "scripting/api.hpp"
#include "assets/api.hpp"

#include "idolon.hpp"

enum : uint8
{
	APP_SHELL = 0,
	APP_TILESET_EDITOR,
	APP_MAP_EDITOR,
	APP_SCRIPT_EDITOR,
	APP_GAME_EDITOR,
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

	//Actions
	void RunFontEditor(const std::string & tilesetName );
	void RunTilesetEditor(const std::string & tilesetName );
	void RunMapEditor(const std::string & mapName);
	void RunScriptEditor(const std::string & scriptName);
	void RunGameEditor(const std::string & cartPath);
	void RunGame(const std::string & gameName);

}