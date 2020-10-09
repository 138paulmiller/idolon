
#include <duktape/duktape.h>
#include "jseval.hpp"
#include "../idolon.hpp"
#include "../sys.hpp"

//https://duktape.org/guide.html

typedef int ( *JsFunction )( duk_context *ctx );

struct JsBinding
{
	const char *name;
	JsFunction func;
	const char *doc;
};


#include "jsbindings.inl"

//https://wiki.duktape.org/howtomodules
namespace 
{
	duk_context *s_ctx;
	JsBinding s_bindings[]= 
	{
		BINDINGS
	};
	const int s_bindingsCount = sizeof( s_bindings ) / sizeof( JsBinding );
}

JsScript::JsScript(const std::string & name)
	:Script(name, SCRIPT_JAVASCRIPT)
{
	m_compiled = false;
}

JsScript::~JsScript()
{
}

void JsScript::compile()
{
	//load code
	m_compiled = JsEval::Execute(code);
}

bool JsScript::call(const std::string & func, TypedArg & ret, const std::vector<TypedArg> & args )
{
	if ( !m_compiled )
	{
		return false;
	}
	if ( !duk_get_global_string( s_ctx, func.c_str() ) )
	{
		ERR( "Js Script error: %s\n", duk_safe_to_string( s_ctx, -1 ) );
		return false;
	}

	for ( TypedArg arg : args )
	{
		switch ( arg.type )
		{
		case ARG_INT:
			duk_push_int(s_ctx, arg.value.i);
			break;
		case ARG_FLOAT:
			duk_push_number(s_ctx, arg.value.f);
			break;
		case ARG_STRING:
			duk_push_string(s_ctx, arg.value.s);
			break;
		}
	}
	duk_call(s_ctx, args.size());
	switch ( ret.type )
	{
		case ARG_INT:
			ret.value.i = duk_to_int(s_ctx, -1);
			break;
		case ARG_FLOAT:
			ret.value.f = duk_to_number(s_ctx, -1);
			break;
		case ARG_STRING:
			ret.value.s =  duk_to_string(s_ctx, -1);
			break;
	}
	//pop return
	duk_pop(s_ctx );
	return true;
}


 //=========================================== PyEval ==================================================

static void my_fatal(void *udata, const char *msg) 
{
    /* Note that 'msg' may be NULL. */
    ERR("Js Script error: %s\n", (msg ? msg : "no message"));
}
//https://wiki.duktape.org/howtomodules

namespace JsEval
{
	void Startup()
	{		
		void *my_udata = (void *) 0xdeadbeef;  /* whatever's most useful, can be NULL */

		s_ctx = duk_create_heap(NULL, NULL, NULL, my_udata, my_fatal);
		
		for ( int i = 0; i < s_bindingsCount; i++ )
		{
			duk_push_c_function(s_ctx, s_bindings[i].func, DUK_VARARGS);
			duk_put_global_string(s_ctx, s_bindings[i].name);
		}

		JsEval::Import( "core.js" );
	}

	void Shutdown()
	{
		duk_destroy_heap(s_ctx);
	}

	bool Import(const std::string & filepath)
	{
		const std::string fullpath = Sys::AssetPath() + "lib/" + filepath;
		
		std::string code;
		if ( !FS::ReadFile(fullpath, code) )
		{
			LOG( "Js Script Import error: Failed to import %s", fullpath.c_str() );
			return false;
		}

		bool success = JsEval::Execute( code );

		return true;
	}

	bool Execute(const std::string & code)
	{
		if (duk_peval_lstring(s_ctx, code.c_str(), code.size()) != 0) {
			printf( "%d, %s\n", code.size(), code.c_str() );
			/* Use duk_safe_to_string() to convert error into string.  This API
			 * call is guaranteed not to throw an error during the coercion.
			 */
			ERR("Js Script Execute error: %s\n", duk_safe_to_string(s_ctx, -1));
			duk_pop(s_ctx);  /* pop result */
			return false;
		}
		return true;
	}
} // namespace Eval