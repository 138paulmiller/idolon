#pragma once
#include "core.h"
#include "commands.h"

namespace Shell
{
	void Startup(const CommandTable & commands);
	void Shutdown();
	void Run();
	void Log(const std::string & msg);
}