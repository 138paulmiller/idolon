#pragma once
#include "core.h"
#include "commands.h"

namespace Shell
{
	void Startup();
	void Shutdown();
	void Run();
	void Log(const std::string & msg);
	void SetFont(const std::string & font);
	void AddCommands(const CommandTable & commands);
}