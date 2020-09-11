
#include <duktape/duktape.h>
#include "jseval.hpp"
//https://duktape.org/guide.html
namespace 
{
	duk_context *s_ctx;
}

JsScript::JsScript(const std::string & name)
	:Script(name)
{
}

JsScript::~JsScript()
{
}

void JsScript::compile()
{

}

bool JsScript::call(const std::string & func, TypedArg & ret, const std::vector<TypedArg> & args )
{

	duk_push_string(s_ctx, func.c_str());
	duk_call(s_ctx, args.size());
	printf("%s -> %s\n", func.c_str(), duk_to_string(s_ctx, -1));

	return false;
}


 //=========================================== PyEval ==================================================

//#include "jsbindings.inl"

static int dummy_upper_case(duk_context *ctx) {
	size_t sz;
	const char *val = duk_require_lstring(ctx, 0, &sz);
	size_t i;

	/* We're going to need 'sz' additional entries on the stack. */
	duk_require_stack(ctx, sz);

	for (i = 0; i < sz; i++) {
		char ch = val[i];
		if (ch >= 'a' && ch <= 'z') {
			ch = ch - 'a' + 'A';
		}
		duk_push_lstring(ctx, (const char *)&ch, 1);
	}

	duk_concat(ctx, sz);
	return 1;
}

namespace JsEval
{
	void Startup()
	{		
		s_ctx = duk_create_heap_default();
		duk_push_c_function(s_ctx, dummy_upper_case, 1);
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