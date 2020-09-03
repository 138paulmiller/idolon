/*
Some notes

 Mouse should be a type 
 :https://docs.python.org/3/extending/newtypes.html
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


#define PYTHON_DEFINES R"(
#layers
BG=0
SP=1
FG=2
UI=3

#keys
ESCAPE = '\033'
SHIFT = 127
ALT = SHIFT + 1
UP = ALT + 1
DOWN = UP + 1
LEFT = DOWN + 1
RIGHT = LEFT + 1
)"


#define PYTHON_BINDINGS \
    PYDECL( idolon, mx, "Get mouse x position")\
	PYDECL( idolon, my, "Get mouse y position")\
	PYDECL( idolon, cls, "Clear screen with color r,g,b")\
	PYDECL( idolon, key, "Get key state. 0 is up, 1 is down. 2 is hold")\



//-------------------------------------------------------------------//
PYBIND( idolon, mx)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	return PyLong_FromLong(x);
}

//-------------------------------------------------------------------//
PYBIND( idolon, my)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	return PyLong_FromLong(y);
}

//-------------------------------------------------------------------//
PYBIND( idolon, cls)
{
	Color c = { 255,0,0,0 };
	if ( PyArg_ParseTuple( args, "iii", &c.r, &c.g, &c.b ) == 0 )
	{
		PYERR( "Expected args (int, int, int)" );
	}
	else
	{
		Engine::ClearScreen( c );
	}
	Py_INCREF(Py_None);
	return Py_None;
}

//-------------------------------------------------------------------//
PYBIND( idolon, key)
{
	Key key;
	if ( PyArg_ParseTuple( args, "i", &key ) == 0 )
	{
		PYERR( "Expected args (int)" );
		Py_INCREF(Py_None);
		return Py_None;
	}
	else
	{
		return PyLong_FromLong(Engine::GetKeyState( key ));
	}
}


