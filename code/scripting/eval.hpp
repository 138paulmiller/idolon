#pragma once 
#include <string>
#include <vector>

#include "../engine/api.hpp"

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
	TypedArg(ArgType type = ARG_NONE);
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

class Script : public Asset
{
public:
	//
	Script(const std::string & name="");
	virtual ~Script() = default;
	virtual void compile();
	virtual bool call(const std::string & func, const std::vector<TypedArg> & args, TypedArg & ret ) ;
	virtual bool deserialize( std::istream& in );
	virtual void serialize( std::ostream& out ) const;

	std::string code;

};

enum ScriptLanguage
{
	SCRIPT_NONE=0, SCRIPT_PYTHON
};


namespace Eval
{
	void Startup(ScriptLanguage lang = SCRIPT_PYTHON);
	void Shutdown();

	void Execute(const std::string & code);
	//
	Script * Compile(const std::string & name);

}