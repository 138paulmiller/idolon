
#include <duktape/duktape.h>
#include "jseval.hpp"
namespace 
{

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
	return false;
}


 //=========================================== PyEval ==================================================

namespace JsEval
{
	void Startup()
	{		
		//set up module initializer. 
	}

	void Shutdown()
	{
	}

	void Execute(const std::string & code)
	{
	}
} // namespace Eval