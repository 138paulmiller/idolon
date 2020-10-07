/* 
notes

 Mouse and sprites should be a type 
 https://docs.python.org/3.5/extending/newtypes.html
or 
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	PyObject *mouse = PyDict_New();
	PyDict_SetItemString(mouse, "x", PyLong_FromLong(x));
	PyDict_SetItemString(mouse, "y", PyLong_FromLong(y));
	return mouse;
	//issue with this impl is that it will be dict, i.e. mouse['x']
*/

#define DECL(module, name, doc )\
	{ #name, py_##module##_##name, METH_VARARGS, doc },

#define BIND(module, name) \
	PyObject* py_##module##_##name(PyObject *self, PyObject *args)



#define ARG_STR_OPT(var)\
	PyArg_ParseTuple( args, "s", &var )
	
#define ARG_STR(var)\
	if ( ARG_STR_OPT(var) == 0 )\
	{\
		LOG("Script : Python : Expected string");\
		return 0;\
	}

#define ARG_INT_OPT(var) \
	PyArg_ParseTuple( args, "i", &var )

#define ARG_INT(var) \
	if ( ARG_INT_OPT(var) == 0 )\
	{\
		LOG("Script : Python : Expected integer");\
		Py_RETURN_NONE;\
	}

#define RET() \
	Py_RETURN_NONE

#define RET_INT(value) \
	return PyLong_FromLong(value);

#define RET_BEG(obj, cnt) \
	int _retIndex = 0;\
	PyObject *obj = PyTuple_New(cnt);

#define RET_SET_INT(obj, id, value ) \
	PyTuple_SetItem( obj, _retIndex++, PyLong_FromLong( value ) );

#define RET_END( obj )\
	Py_XINCREF( obj );\
	return obj;

#include "bindings.inl"


BIND(idolon, log)
{
	const int LOG_MAX = 2048;

	char buffer[LOG_MAX];
	char *str = buffer;
	int i;
	char *s;
	bool found = false;
	do{
		if ( str - buffer > LOG_MAX ) break;

		found = ARG_INT_OPT( i );
		if ( found )
		{
			str += sprintf_s( str, LOG_MAX, "%s ", i );
		}
		else
		{ 
			found = ARG_STR_OPT( s );
			if ( found )
			{
				str += sprintf_s( str, LOG_MAX, "%s ", s );
			}
		}
	} while ( found );

	LOG( buffer );
	LOG( "\n" );
	RET();
}