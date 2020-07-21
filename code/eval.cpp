#include "pch.hpp"

#include "sys.hpp"
#include "eval.hpp"
#include <Python.h>

namespace 
{
	//////////////////// Begin Module //////////////////

	//Python module
	static int numargs=100;
	/* Return the number of arguments of the application command line */
	static PyObject* emb_numargs(PyObject *self, PyObject *args)
	{
	    if(!PyArg_ParseTuple(args, ":numargs"))
	        return NULL;
	    return PyLong_FromLong(numargs);
	}

	static PyMethodDef EmbMethods[] = {
	    {"numargs", emb_numargs, METH_VARARGS,
	     "Return the number of arguments received by the process."},
	    {NULL, NULL, 0, NULL}
	};

	static PyModuleDef EmbModule = {
	    PyModuleDef_HEAD_INIT, "emb", NULL, -1, EmbMethods,
	    NULL, NULL, NULL, NULL
	};

	static PyObject* PyInit_emb(void)
	{
	    return PyModule_Create(&EmbModule);
	}
	//////////////////// End Module //////////////////
	

	void SetupBindingsPython()
	{
		//set up bindings that can be called in python. 
    	PyImport_AppendInittab("emb", &PyInit_emb);
	}	
	

	void StartupPython()
	{
		SetupBindingsPython();
		Py_Initialize();
		//set import dir to cwd 
		PyRun_SimpleString("import sys\nimport os\n"); 
		//create a scripts dir in systm
		//todo when parsing game code - write it all into a single python file in a temp system dir
		std::string cmd = "sys.path.append('" +  Sys::Path() + "')\n";

		PyRun_SimpleString(cmd.c_str());

	}

	void ShutdownPython()
	{
		Py_Finalize();
	}


	//call pyhton code with arguments
	//file.py funcname [args]
	int execute(int argc, const char *argv[])
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
	    /* Error checking of pName left out */

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
	                    fprintf(stderr, "Cannot convert argument\n");
	                    return 1;
	                }
	                /* pValue reference stolen here: */
	                PyTuple_SetItem(pArgs, i, pValue);
	            }
	            pValue = PyObject_CallObject(pFunc, pArgs);
	            Py_DECREF(pArgs);
	            if (pValue != NULL) {
	                printf("Result of call: %ld\n", PyLong_AsLong(pValue));
	                Py_DECREF(pValue);
	            }
	            else {
	                Py_DECREF(pFunc);
	                Py_DECREF(pModule);
	                PyErr_Print();
	                fprintf(stderr,"Call failed\n");
	                return 1;
	            }
	        }
	        else {
	            if (PyErr_Occurred())
	                PyErr_Print();
	            fprintf(stderr, "Cannot find function \"%s\"\n", funcname);
	        }
	        Py_XDECREF(pFunc);
	        Py_DECREF(pModule);
	    }
	    else {
	        PyErr_Print();
	        fprintf(stderr, "Failed to load \"%s\"\n", file.c_str());
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

namespace Eval
{
	void Startup()
	{		
		StartupPython();

	   	//execute. 
	    PyRun_SimpleString("from time import time,ctime\n"
							"import emb\n"
	                       "print('Today is', ctime(time()))\n"
							"print('Number of arguments', emb.numargs())\n"
		);


		//numargs = 4;
		const char * args[] = {"test", "multiply", "3", "2"};
		execute(4 , args);

	}
	void Shutdown()
	{
		ShutdownPython();
	}

	void Load(const std::string & file)
	{}

	void Execute(const std::string & code)
	{

	}
}