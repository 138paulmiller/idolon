#pragma once
#include <functional>

using Job = std::function<void()>;

namespace Pool
{
	void Startup();
	void Run(const Job & job);
	void Shutdown();

}