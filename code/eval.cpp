#include "pch.hpp"

#include "sys.hpp"
#include "eval.hpp"

#define PY_SSIZE_T_CLEAN
#undef _DEBUG
#include <Python.h>

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

namespace 
{

	//map form filename to module
	
	std::unordered_map<std::string, PyObject* > s_fileToModule;
	std::unordered_map<std::string, PyObject* > s_funcs;

	#include "bindings.inl"
	/*
	const char * name, //name of the method
	PyCFunction method //pointer to the C implementation PyObject* name(PyObject *self, PyObject *args)
	int flags          //flag bits indicating how the call should be constructed
	const char * doc   //points to the contents of the docstring
	*/
	static PyMethodDef s_pymethods[] =
	{
		PYTHON_BINDINGS
	    {0, 0, 0, 0}
	};

	static PyModuleDef s_pymodule = 
	{
	    PyModuleDef_HEAD_INIT, 
		SYSTEM_NAME,     						/* m_name */
		"API to the " SYSTEM_NAME " system ",  	/* m_doc */
		-1,                  	/* m_size */
		s_pymethods,    		/* m_methods */
		NULL,                	/* m_reload */
		NULL,                	/* m_traverse */
		NULL,                	/* m_clear */
		NULL,                	/* m_free */
	};

	static PyObject* InitModule(void)
	{
	    return PyModule_Create(&s_pymodule);
	}

}



namespace Eval
{
	void Startup()
	{		
		//set up module initializer. 
    	PyImport_AppendInittab(SYSTEM_NAME, &InitModule);

		Py_Initialize();

		//add system path for imports
		const std::string & cmd = "import sys\nimport os\nsys.path.append('" +  Sys::Path() + "')\n";
		PyRun_SimpleString(cmd.c_str());

	}
	void Shutdown()
	{
		for ( auto pair : s_fileToModule )
		{
			Py_DECREF(pair.second);
		}
		for ( auto pair : s_funcs)
		{
			Py_DECREF(pair.second);
		}
		Py_Finalize();
	}

	void Compile(const std::string & file)
	{
		LOG("Compiling %s\n", file.c_str());

		PyObject * modname = PyUnicode_FromString(file.c_str());
		if ( s_fileToModule.find( file ) != s_fileToModule.end() )
		{
			Py_DECREF(s_fileToModule[file]);
		}
	    PyObject *const module = s_fileToModule[file] = PyImport_Import(modname);
	    if(!module)
	    {
	        PyErr_Print();
	        LOG("Eval: Could not import %s\n", file.c_str());
	        return;	    	
	    }

		PyObject *const pDict = PyModule_GetDict(module); // borrowed
		// find functions
		PyObject *key = nullptr, *value = nullptr;
		for (Py_ssize_t i = 0; PyDict_Next(pDict, &i, &key, &value);) 
		{
			//if function
			if (PyFunction_Check(value)) 
			{
				s_funcs[PyUnicode_AsUTF8(key)] = value;
			}
		}
	    Py_DECREF(modname);
	}

	void Execute(const std::string & code)
	{
	    PyRun_SimpleString(code.c_str());
	}

	bool Call(const std::string & func, const std::vector<TypedArg> & args, TypedArg & ret )
	{
		if ( s_funcs.find( func) == s_funcs.end() )
		{
			LOG( "Eval : Function %s not loaded\n", func.c_str() );
			return 0;
		}
		PyObject* value;
		PyObject* funcobj = s_funcs[func.c_str()];
		//create tuple to pass into func
		PyObject* argsobj = PyTuple_New(args.size());
		int i = 0;
	    for (const TypedArg & arg : args ) {
			switch ( arg.type )
			{
			case ARG_INT:
				value= PyLong_FromLong(arg.value.i);
				break;
			case ARG_FLOAT:
				value= PyFloat_FromDouble(arg.value.f);
				break;
			case ARG_STRING:
				value=  PyUnicode_FromString(arg.value.s);
				break;
			default :
				value = 0;
				break;
			}
	        if (!value) {
	            Py_DECREF(argsobj);
	            LOG("Eval: Cannot convert argument\n");
	            return 0;
	        }
	        PyTuple_SetItem(argsobj, i, value);
			i++;
	    }
	    value = PyObject_CallObject(funcobj, argsobj);
	    Py_DECREF(argsobj);
	    if (value != NULL) 
		{
	    	switch ( ret.type )
			{
			case ARG_INT:
				ret.value.i = PyLong_AsLong(value);
				break;
			case ARG_FLOAT:
				ret.value.f = PyFloat_AsDouble(value);
				break;
			case ARG_STRING:
				ret.value.s =  PyUnicode_AsUTF8(value);
				break;
			}
	        Py_DECREF(value);
	    }
	    else 
		{
	        PyErr_Print();
	        LOG("Eval: Call failed\n");
	        return 0;
	    }
		return 1;
	}
}