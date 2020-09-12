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

enum ScriptLanguage
{
	SCRIPT_NONE=0, SCRIPT_PYTHON, SCRIPT_JAVASCRIPT
};

ScriptLanguage ScriptLanguageFromStr(const std::string & str);
const std::string ScriptLanguageToStr(ScriptLanguage lang);



class Script : public Asset
{
public:
	Script( const std::string & name, ScriptLanguage lang = SCRIPT_NONE);
	virtual ~Script() = default;
	virtual void compile();
	virtual bool call( const std::string &func, TypedArg &ret, const std::vector<TypedArg> &args = {} );

	std::string code;
	ScriptLanguage lang;
};

namespace Eval
{
	void Startup(ScriptLanguage lang = SCRIPT_PYTHON);
	void Shutdown();

	void Reset(ScriptLanguage lang = SCRIPT_PYTHON);
	
	void Execute(const std::string & code);

	Script * CreateScript(const std::string name, ScriptLanguage lang);
}