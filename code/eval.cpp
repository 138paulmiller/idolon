#include "pch.hpp"

#include "sys.hpp"
#include "eval.hpp"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

namespace 
{


#define DEF_PYTHON_BINDING(module, name)\
	PyObject* module##_##name(PyObject *self, PyObject *args)

#define ADD_PYTHON_BINDING(module, name,doc)\
	s_pymethods[index++] = {#name, module##_##name, METH_VARARGS, doc};
#define NUM_PYTHON_BINDINGS 1

	DEF_PYTHON_BINDING(idolon, mouse)
	{
		static int x = 0;
		static int y = 0;
		Engine::GetMousePosition( x, y);
		PyObject *mouse = PyDict_New();
		PyDict_SetItemString(mouse, "x", PyLong_FromLong(x));
		PyDict_SetItemString(mouse, "y", PyLong_FromLong(y));
		return mouse;
	}


	/////////////////////Begin Module ////////////////
	/*
	const char * name, //name of the method
	PyCFunction method //pointer to the C implementation PyObject* name(PyObject *self, PyObject *args)
	int flags          //flag bits indicating how the call should be constructed
	const char * doc   //points to the contents of the docstring
	*/
	static PyMethodDef s_pymethods[NUM_PYTHON_BINDINGS+1] = 
	{
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
	//////////////////// End Module //////////////////

	void SetupPythonBindings()
	{	
		int index = 0;
		ADD_PYTHON_BINDING(idolon, mouse, "Returns mouse data");
	}


	void StartupPython()
	{

		SetupPythonBindings();

		//set up module initializer. 
    	PyImport_AppendInittab(SYSTEM_NAME, &InitModule);

		Py_Initialize();

		//add system path for imports
		PyRun_SimpleString("import sys\nimport os\n"); 
		//todo when parsing game code add its
		const std::string & cmd = "sys.path.append('" +  Sys::Path() + "')\n";
		PyRun_SimpleString(cmd.c_str());
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

namespace Eval
{
	void Startup()
	{		
		StartupPython();

	   	//execute. 
		//numargs = 4;
		const char * args[] = {"test", "multiply", "3", "2"};
		CallPython(4 , args);

	}
	void Shutdown()
	{
		ShutdownPython();
	}

	void Load(const std::string & file)
	{}

	void Execute(const std::string & code)
	{
	    PyRun_SimpleString(code.c_str());
	}

	void Call(const std::string & func, const std::string * args)
	{

	}
}