#include "pch.hpp"

#include "sys.hpp"
#include "eval.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

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


	void StartupPython()
	{
		//set up module initializer. 
    	PyImport_AppendInittab(SYSTEM_NAME, &InitModule);

		Py_Initialize();

		//add system path for imports
		const std::string & cmd = "import sys\nimport os\nsys.path.append('" +  Sys::Path() + "')\n";
		PyRun_SimpleString(cmd.c_str());
		//todo when parsing game code add its

	}

	void ShutdownPython()
	{
		Py_Finalize();
	}


	//call pyhton code with arguments
	//file.py funcname [args]
	int CallPython(int argc, const char *argv[])
	{
		if (argc < 2) {
	        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
	        return 1;
	    }
		const std::string file = argv[0] ;
		const char * funcname = argv[1];

		argv+=2;
		argc-=2;

	    PyObject *pName, *pModule, *pDict, *pFunc;
	    PyObject *pArgs, *pValue;
	    int i;

	    pName = PyUnicode_DecodeFSDefault(file.c_str());
	    pModule = PyImport_Import(pName);
	    Py_DECREF(pName);

	    if (pModule != NULL) {
	        pFunc = PyObject_GetAttrString(pModule, funcname);
	        /* pFunc is a new reference */

	        if (pFunc && PyCallable_Check(pFunc)) {
	            pArgs = PyTuple_New(argc);
	            for (i = 0; i < argc; ++i) {
	                pValue = PyLong_FromLong(atoi(argv[i]));
	                if (!pValue) {
	                    Py_DECREF(pArgs);
	                    Py_DECREF(pModule);
	                    fprintf(stderr, "Eval: Cannot convert argument\n");
	                    return 1;
	                }
	                /* pValue reference stolen here: */
	                PyTuple_SetItem(pArgs, i, pValue);
	            }
	            pValue = PyObject_CallObject(pFunc, pArgs);
	            Py_DECREF(pArgs);
	            if (pValue != NULL) {
	                printf("Eval: Result of call: %ld\n", PyLong_AsLong(pValue));
	                Py_DECREF(pValue);
	            }
	            else {
	                Py_DECREF(pFunc);
	                Py_DECREF(pModule);
	                PyErr_Print();
	                fprintf(stderr,"Eval: Call failed\n");
	                return 1;
	            }
	        }
	        else {
	            if (PyErr_Occurred())
	                PyErr_Print();
	            fprintf(stderr, "Eval: Cannot find function \"%s\"\n", funcname);
	        }
	        Py_XDECREF(pFunc);
	        Py_DECREF(pModule);
	    }
	    else {
	        PyErr_Print();
	        fprintf(stderr, "Eval: Failed to load \"%s\"\n", file.c_str());
	        return 1;
	    }
	    return 0;
	}

}

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


namespace Eval
{
	void Startup()
	{		
		StartupPython();

		//const char * args[] = {"test", "multiply", "3", "2"};
		//CallPython(4 , args);

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
		ShutdownPython();
	}

	void Compile(const std::string & file)
	{
		PyObject * modname = PyUnicode_FromString(file.c_str());
		if ( s_fileToModule.find( file ) != s_fileToModule.end() )
		{
			Py_DECREF(s_fileToModule[file]);
		}
	    PyObject *const module = s_fileToModule[file] = PyImport_Import(modname);
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
			printf( "Eval : Function %s not loaded ", func.c_str() );
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
	            printf("Eval: Cannot convert argument\n");
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
	        printf("Eval: Call failed\n");
	        return 0;
	    }
		return 1;
	}
}