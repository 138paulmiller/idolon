
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

TypedArg::TypedArg( const char* s )
	:type( ARG_STRING )
{
	value.s = s;
}


Script::Script( const std::string &name, ScriptLanguage lang  )
	: Asset(name)
	, lang(lang)
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
	const std::string s_langEnumToStr[] = 
	{
		"none",
		"python",
		"javascript"
	};

	const std::unordered_map<std::string, ScriptLanguage > s_langStrToEnum =
	{
		{ s_langEnumToStr[SCRIPT_NONE]       , SCRIPT_NONE       },
		{ s_langEnumToStr[SCRIPT_PYTHON]     , SCRIPT_PYTHON     },
		{ s_langEnumToStr[SCRIPT_JAVASCRIPT ], SCRIPT_JAVASCRIPT },
	};


	ScriptLanguage s_lang;
	typedef void(*StartupFunc)() ;
	typedef void(*ShutdownFunc)();
	typedef bool(*ExecuteFunc)(const std::string & code);

	StartupFunc s_startupImpl = 0 ;
	ShutdownFunc s_shutdownImpl = 0;
	ExecuteFunc s_executeImpl = 0;

	bool s_initialized = false;

}

ScriptLanguage ScriptLanguageFromStr(const std::string & str)
{
	auto it = s_langStrToEnum.find(str);
	if (it == s_langStrToEnum.end())
	{
		return SCRIPT_NONE;
	}
	return it->second;
}


const std::string ScriptLanguageToStr( ScriptLanguage lang )
{
	return s_langEnumToStr[lang];
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
		s_initialized = true;

		s_startupImpl();
	}
	void Shutdown()
	{
		if ( s_initialized )
		{
			s_shutdownImpl();
			s_initialized = false;
		}

		s_startupImpl = 0;
		s_shutdownImpl = 0;
		s_executeImpl = 0;
	}
	void Reset(ScriptLanguage lang)
	{
		Shutdown();
		Startup(lang);
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