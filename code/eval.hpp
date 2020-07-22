#pragma once 
#include "core.hpp"

/*
	Code evaluator
*/

//Add usage string to command ,with expected arg count. throw excpetion message with error. then have main forward this to shell 

namespace Eval
{
	void Startup();
	void Shutdown();
	//each frame call update() then draw(). 
	void Load(const std::string & file);
	void Execute(const std::string & code);
	//compile game code. get handles to update and draw
	void Call(const std::string & func, const std::string * args);
}