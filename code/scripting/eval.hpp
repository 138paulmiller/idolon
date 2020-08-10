#pragma once 
#include <string>
#include <vector>

/*
	Code evaluator
*/

//Add usage string to command ,with expected arg count. throw excpetion message with error. then have main forward this to shell 
enum ArgType
{
	ARG_NONE=0, ARG_INT, ARG_FLOAT, ARG_STRING
};

struct TypedArg
{
	TypedArg(ArgType type);
	TypedArg(int i );
	TypedArg(float f);
	TypedArg( char* s );
	union Value
	{
		int i ;
		float f ;
		const char* s ;
	} 
	value;
	
	ArgType type = ARG_NONE;
};
namespace Eval
{
	void Startup();
	void Shutdown();
	//compile game code. cannot redefine functions. all
	void Compile(const std::string & file);
	void Execute(const std::string & code);
	//game calls update and draw. Uses ret type to parse return
	bool Call(const std::string & func, const std::vector<TypedArg> & args, TypedArg & ret);
}