#include "pch.hpp"

#include "eval.hpp"

#include <Python.h>

namespace 
{
	//Python module
	static int numargs=0;
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

	//file.py funcname [args]
	int execute(int argc, char *argv[])
	{
		if (argc < 2) {
	        fprintf(stderr,"Usage: call pythonfile funcname [args]\n");
	        return 1;
	    }

		const char * file = argv[0];
		const char * funcname = argv[1];
		argv+=2;
		argc-=2;

	    PyObject *pName, *pModule, *pDict, *pFunc;
	    PyObject *pArgs, *pValue;
	    int i;

	    
	    Py_Initialize();
	    pName = PyUnicode_DecodeFSDefault(file);
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
	        fprintf(stderr, "Failed to load \"%s\"\n", file);
	        return 1;
	    }
	    Py_Finalize();
	    return 0;
	}

}

/*
	TODO 
	- Get handles to to all event functions
	- grab update and draw handles in game code

	- Create Api function bindings for idolon

*/

namespace Eval
{
	void Startup()
	{
		numargs = 4;
		char * args[] = {"test", "multiply", "3", "2"};
		execute(4 , args);

		//set up bindings
    	PyImport_AppendInittab("emb", &PyInit_emb);
	    Py_Initialize();
	   	//execute. 
	    PyRun_SimpleString("from time import time,ctime\n"
	                       "print('Today is', ctime(time()))\n"
							"import emb\n"
							"print('Number of arguments', emb.numargs())\n"
		);

	}
	void Shutdown()
	{

	}

	void Load(const std::string & file)
	{}

	void Execute(const std::string & code)
	{

	}
}