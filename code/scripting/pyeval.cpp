

#include "pyeval.hpp"

#include "../sys.hpp"

#define PY_SSIZE_T_CLEAN
#undef _DEBUG
#include <Python.h>
//map form filename to module

/*
const char * name, //name of the method
PyCFunction method //pointer to the C implementation PyObject* name(PyObject *self, PyObject *args)
int flags          //flag bits indicating how the call should be constructed
const char * doc   //points to the contents of the docstring
*/



namespace 
{

	#include "pybindings.inl"

	PyMethodDef s_pymethods[]
	{	
		BINDINGS
		{0,0,0,0}
	};

	PyModuleDef s_pymodule = 
	{
	    PyModuleDef_HEAD_INIT, 
		SYSTEM_NAME,     						// m_name 
		"API to the " SYSTEM_NAME " system ",  	// m_doc 
		-1,                  	                // m_size 
		s_pymethods,    		                // m_methods 
		NULL,                	                // m_reload 
		NULL,                	                // m_traverse 
		NULL,                	                // m_clear 
		NULL,                	                // m_free 
	};

	PyObject* InitModule(void)
	{
	    return PyModule_Create(&s_pymodule);
	}

}


PyScript::PyScript(const std::string & name)
	:Script(name, SCRIPT_PYTHON)
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

	LOG("Compiling %s\n", name.c_str());
	if ( code.size() == 0 ) return;
	//temp file 
	const std::string &modulename = name;
	const std::string & tempfilename = modulename + ".py";
	const std::string & temppath = FS::Append(FS::Cwd(), tempfilename);
	const std::string & tempdir  = FS::DirName( temppath);

	std::ofstream tempfile( temppath );
	if ( !tempfile.is_open() )
	{
        LOG("Eval: Could not create temp file %s\n", temppath.c_str());
		return;
	}


	tempfile << BINDINGS_CONSTS << code;
	tempfile.close();

	//set execute dir
	const std::string &prelude = "import sys\nsys.path.append('" + tempdir + "')\n";
	PyRun_SimpleString(prelude .c_str());

	for ( auto pair : m_funcs)
	{
		Py_DECREF(pair.second);
	}
	m_funcs.clear();
	if ( m_module )
	{
		Py_DECREF(m_module);
	}	
    m_module = PyImport_ImportModule(modulename.c_str());

	 PyErr_Print();

    if(!m_module)
    {
        PyErr_Print();
        LOG("Eval: Could not import %s\n", name.c_str());
    }
	else
	{

		// find all functions?
		/*
		PyObject *const pDict = PyModule_GetDict(m_module); // borrowed
		PyObject *key = nullptr, *value = nullptr;
		for (Py_ssize_t i = 0; PyDict_Next(pDict, &i, &key, &value); i++) 
		{
			const char *const name = PyUnicode_AsUTF8( key );
			//if function
			if (PyFunction_Check(value)) 
			{
				m_funcs[name] = value;
			}
		}
		*/
		const char * api[] = { GAME_API_UPDATE, GAME_API_ONKEY, 0 };
		for ( const char **funcptr = api; *funcptr; funcptr++ )
		{
			PyObject *attr = PyObject_GetAttrString( m_module, *funcptr );
			if ( attr )
			{
				m_funcs[*funcptr] = attr;
			}
			else
			{
				LOG("Script:%s %s is missing!\n", name.c_str(), *funcptr );
			}
		}

	}
	const std::string &postlude = "import sys\nsys.path.pop();";
	PyRun_SimpleString(postlude .c_str());

	//remove all dirs
	FS::Remove( tempfilename );

}

bool PyScript::call(const std::string & func, TypedArg & ret, const std::vector<TypedArg> & args )
{
	if ( m_funcs.find( func) == m_funcs.end() )
	{
		LOG( "Eval : Function %s not loaded\n", func.c_str() );
		return 0;
	}
	PyObject* value;
	PyObject* funcobj = m_funcs[func.c_str()];
	if ( !funcobj ) return false;
	//create tuple to pass into func

	PyObject* argsobj = args.size() ? PyTuple_New(args.size()) : nullptr;
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
        LOG("Eval: Calling %s failed\n", func.c_str());
        return 0;
    }

	return 1;
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