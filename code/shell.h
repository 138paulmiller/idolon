#pragma once
#include "core.h"
#include "commands.h"

namespace Shell
{
	void Startup();
	void Shutdown();
	void Run();
	void OverrideInput(const std::string& msg);
	void Log(const std::string & msg);
	void SetFont(const std::string & font);
	void AddCommands(const CommandTable & commands);
}