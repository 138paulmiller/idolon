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


#define PYTHON_BINDINGS \
    PYDECL( idolon, mx, "Get mouse x position")\
	PYDECL( idolon, my, "Get mouse y position")\
	PYDECL( idolon, cls, "Clear screen with color r,g,b")\



PYBIND( idolon, mx)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	return PyLong_FromLong(x);
}

PYBIND( idolon, my)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	return PyLong_FromLong(y);
}

PYBIND( idolon, cls)
{
	Color c = { 255,0,0,0 };
	if ( PyArg_ParseTuple( args, "iii", &c.r, &c.g, &c.b ) == 0 )
	{
		PYERR( "Expected (char, char, char)" );
	}
	else
	{
		Engine::ClearScreen( c );
	}
	Py_INCREF(Py_None);
	return Py_None;
  
}


