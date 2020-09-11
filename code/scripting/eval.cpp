
#include "pyeval.hpp"

#include "../sys.hpp"

#define SCRIPT_PRELUDE '$'

/*
	TODO 
	- Get handles to to all event functions
	- grab update and draw handles from game code

	- Create Api function bindings for idolon
		- pFunc = PyObject_GetAttrString(pModule, funcname);
*/


TypedArg::TypedArg( ArgType type )
	: type(type)
{
	value.i = 0;
}

TypedArg::TypedArg( int i )
	:type( ARG_INT )
{
	value.i = i;
}

TypedArg::TypedArg( float f )
	:type( ARG_FLOAT)
{
	value.f = f;
}

TypedArg::TypedArg( char* s )
	:type( ARG_STRING )
{
	value.s = s;
}


Script::Script( const 	std::string &name )
	:Asset(name)
{
}

void Script::compile()
{
	ASSERT(0, "Script: Language cannot compile! Language does not support it");
}

bool Script::call(const std::string & func, TypedArg & ret, const std::vector<TypedArg> & args ) 
{
	ASSERT(0, "Script: Language cannot compile! Language does not support it");
	return 0;
}

namespace 
{
	const std::unordered_map<std::string, ScriptLanguage > LangStrToEnum =
	{
		{ "none", SCRIPT_NONE },
		{ "python", SCRIPT_PYTHON },
		{ "javascript", SCRIPT_JAVASCRIPT },
	};

	ScriptLanguage s_lang;
	typedef void(*StartupFunc)() ;
	typedef void(*ShutdownFunc)();
	typedef void(*ExecuteFunc)(const std::string & code);

	StartupFunc s_startupImpl = 0 ;
	ShutdownFunc s_shutdownImpl = 0;
	ExecuteFunc s_executeImpl = 0;
}

ScriptLanguage ScriptLanguageFromStr(const std::string & str)
{
	auto it = LangStrToEnum.find(str);
	if (it == LangStrToEnum.end())
	{
		return SCRIPT_NONE;
	}
	return it->second;
}

namespace Eval
{
	void Startup( ScriptLanguage lang )
	{
		s_lang = lang;
		switch ( s_lang )
		{
		case SCRIPT_PYTHON:
			s_startupImpl = PyEval::Startup;
			s_shutdownImpl = PyEval::Shutdown;
			s_executeImpl = PyEval::Execute;
			break;
		case SCRIPT_JAVASCRIPT:
			s_startupImpl = JsEval::Startup;
			s_shutdownImpl = JsEval::Shutdown;
			s_executeImpl = JsEval::Execute;
			break;
		default:
			break;
		}
		s_startupImpl();
	}
	void Shutdown()
	{
		s_shutdownImpl();

		s_startupImpl = 0;
		s_shutdownImpl = 0;
		s_executeImpl = 0;
	}
	void Reset()
	{
		s_shutdownImpl();
		s_startupImpl();
	}

	void Execute(const std::string & code)
	{
		s_executeImpl(code);
	}

	Script * CreateScript(const std::string name, ScriptLanguage lang)
	{
		switch (lang)
		{
		case SCRIPT_PYTHON:
			return  new PyScript(name);
			break;
		case SCRIPT_JAVASCRIPT:
			return  new JsScript(name);
			break;
		}
		return 0;
	}

} // namespace Eval