
#include <duktape/duktape.h>
#include "jseval.hpp"
//https://duktape.org/guide.html

typedef int ( *JsFunction )( duk_context *ctx );

struct JsBinding
{
	const char *name;
	JsFunction func;
	const char *doc;
};

#define JSDECL(module, name, doc )\
	{ #name, js_##module##_##name, doc } ,

#define JSBIND(module, name) \
	int js_##module##_##name(duk_context *ctx)


#define PYERR(...) \
	LOG(__VA_ARGS__)


#include "jsbindings.inl"

//https://wiki.duktape.org/howtomodules
namespace 
{
	duk_context *s_ctx;
	JsBinding s_bindings[]= 
	{
		JS_BINDINGS
	};
	const int s_bindingsCount = sizeof( s_bindings ) / sizeof( JsBinding );
}

JsScript::JsScript(const std::string & name)
	:Script(name, SCRIPT_JAVASCRIPT)
{
}

JsScript::~JsScript()
{
}

void JsScript::compile()
{
	//load code
	if ( duk_peval_lstring(s_ctx, code.c_str(), code.size()) != 0 ) {
		/* Use duk_safe_to_string() to convert error into string.  This API
		 * call is guaranteed not to throw an error during the coercion.
		 */
		LOG( "Script error: %s\n", duk_safe_to_string( s_ctx, -1 ) );
	}
}

bool JsScript::call(const std::string & func, TypedArg & ret, const std::vector<TypedArg> & args )
{
	if ( !duk_get_global_string( s_ctx, func.c_str() ) )
	{
		LOG( "Script error: %s\n", duk_safe_to_string( s_ctx, -1 ) );
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
	return true;
}


 //=========================================== PyEval ==================================================


namespace JsEval
{
	void Startup()
	{		
		s_ctx = duk_create_heap_default();
		for ( int i = 0; i < s_bindingsCount; i++ )
		{
			duk_push_c_function(s_ctx, s_bindings[i].func, DUK_VARARGS);
			duk_put_global_string(s_ctx, s_bindings[i].name);
		}

	}

	void Shutdown()
	{
		duk_pop(s_ctx);
		duk_destroy_heap(s_ctx);
	}

	void Execute(const std::string & code)
	{
	}
} // namespace Eval