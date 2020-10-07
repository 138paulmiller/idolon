#pragma once 

#include "eval.hpp"


class JsScript : public Script
{
public:
	//
	JsScript(const std::string & name="");
	~JsScript();
	void compile() override;
	bool call( const std::string &func, TypedArg &ret, const std::vector<TypedArg> &args = {} ) override;


private:
	bool m_compiled;
};



namespace JsEval
{
	void Startup();
	void Shutdown();
	bool Execute(const std::string & code);

}