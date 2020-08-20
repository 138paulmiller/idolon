

#include "pyeval.hpp"

#include "../sys.hpp"

#define PY_SSIZE_T_CLEAN
#undef _DEBUG
#include <Python.h>
//map form filename to module


	//
PyScript::PyScript(const std::string & name)
	:Script(name)
{
	m_module = 0;
	compile();
}

PyScript::~PyScript()
{
	if ( m_module )
	{
		Py_DECREF(m_module);
	}
	for ( auto pair : m_funcs)
	{
		Py_DECREF(pair.second);
	}
}

void PyScript::compile()
{
	
	const std::string & file = name;
	LOG("Compiling %s\n", file.c_str());
	if ( code.size() == 0 ) return;

	PyObject * modname = PyUnicode_FromString(file.c_str());
	if ( m_module )
	{
		Py_DECREF(m_module);
	}
    m_module = PyImport_Import(modname);
    if(!m_module)
    {
        PyErr_Print();
        LOG("Eval: Could not import %s\n", file.c_str());
        return;	    	
    }

	PyObject *const pDict = PyModule_GetDict(m_module); // borrowed
	// find functions
	PyObject *key = nullptr, *value = nullptr;
	for (Py_ssize_t i = 0; PyDict_Next(pDict, &i, &key, &value);) 
	{
		//if function
		if (PyFunction_Check(value)) 
		{
			m_funcs[PyUnicode_AsUTF8(key)] = value;
		}
	}
    Py_DECREF(modname);
  	if(m_funcs.find(GAME_API_INIT) != m_funcs.end())
	{
		LOG("Script: %s is missing init function", name.c_str());
	}
	if(m_funcs.find(GAME_API_UPDATE) != m_funcs.end())
	{
		LOG("Script: %s is missing update function", name.c_str());
	}
}

bool PyScript::call(const std::string & func, const std::vector<TypedArg> & args, TypedArg & ret )
{
	if ( m_funcs.find( func) == m_funcs.end() )
	{
		LOG( "Eval : Function %s not loaded\n", func.c_str() );
		return 0;
	}
	PyObject* value;
	PyObject* funcobj = m_funcs[func.c_str()];
	//create tuple to pass into func
	PyObject* argsobj = PyTuple_New(args.size());
	int i = 0;
    for (const TypedArg & arg : args ) 
    {
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
        if (!value) 
        {
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

namespace 
{
	#include "pybindings.inl"
	/*
	const char * name, //name of the method
	PyCFunction method //pointer to the C implementation PyObject* name(PyObject *self, PyObject *args)
	int flags          //flag bits indicating how the call should be constructed
	const char * doc   //points to the contents of the docstring
	*/
	PyMethodDef s_pymethods[] =
	{
		PYTHON_BINDINGS
	    {0, 0, 0, 0}
	};

	PyModuleDef s_pymodule = 
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

	PyObject* InitModule(void)
	{
	    return PyModule_Create(&s_pymodule);
	}

}
 //=========================================== PyEval ==================================================

namespace PyEval
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
		Py_Finalize();
	}

	void Execute(const std::string & code)
	{
	    PyRun_SimpleString(code.c_str());
	}
} // namespace Eval