
#include "pyeval.hpp"

#include "../sys.hpp"


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

Script::Script(const std::string & name)
	:Asset(name)
{}

void Script::compile()
{
	ASSERT(0, "Script: Language cannot compile! Language does not support it");
}

bool Script::call(const std::string & func, const std::vector<TypedArg> & args, TypedArg & ret ) 
{
	ASSERT(0, "Script: Language cannot compile! Language does not support it");
}

bool Script::deserialize( std::istream& in )
{
	try
	{
		std::getline( in, name, '\n' ); 
		FS::ReplaceAll(name, "\r", "");
		int len;
		in >> len ;
		char * data = new char[len]; 
		in.read(data, len);
		code = std::string(data);
		delete data;
		//recompile
		compile();
		return true;
	}
	catch(...)
	{
		LOG("Script : failed to load script %s", name.c_str());
		return false;
	}
}

void Script::serialize( std::ostream& out ) const
{
    out << name << std::endl;
	out << code.size() << code;
}

namespace 
{
	ScriptLanguage s_lang;
}

namespace Eval
{
	void Startup(ScriptLanguage lang)
	{		
		s_lang = lang;
		switch(s_lang)
		{
			case SCRIPT_PYTHON:
				PyEval::Startup();
			break;

			default:
			break;
		}

	}
	void Shutdown()
	{
		switch(s_lang)
		{
			case SCRIPT_PYTHON:
				PyEval::Shutdown();
			break;

			default:
			break;
		}
	}

	void Execute(const std::string & code)
	{
		switch(s_lang)
		{
			case SCRIPT_PYTHON:
				PyEval::Execute(code);
			break;

			default:
			break;
		}
	}
	
} // namespace Eval