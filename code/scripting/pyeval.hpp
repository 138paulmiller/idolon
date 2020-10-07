#pragma once 

#include "eval.hpp"

//forward declar PyObject. Uses
#ifndef PyObject_HEAD
struct _object;
typedef _object PyObject;
#endif

class PyScript : public Script
{
public:
	//
	PyScript(const std::string & name="");
	~PyScript();
	void compile() override;
	bool call( const std::string &func, TypedArg &ret, const std::vector<TypedArg> &args = {} ) override;


private:
	PyObject* m_module;
	std::unordered_map<std::string, PyObject* > m_funcs;
};



namespace PyEval
{
	void Startup();
	void Shutdown();
	bool Execute(const std::string & code);

}