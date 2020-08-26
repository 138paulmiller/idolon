


#define PYBIND(module, name) \
	PyObject* module##_##name(PyObject *self, PyObject *args)

#define PYINIT(module, name, doc )\
	{ #name, module##_##name, METH_VARARGS, doc },


#define PYTHON_BINDINGS \
    PYINIT( idolon, mx, "Get mouse x position")\
	PYINIT( idolon, my, "Get mouse y position")


/*
 Mouse should be a type https://docs.python.org/3/extending/newtypes.html
or use 
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	PyObject *mouse = PyDict_New();
	PyDict_SetItemString(mouse, "x", PyLong_FromLong(x));
	PyDict_SetItemString(mouse, "y", PyLong_FromLong(y));
	return mouse;

*/

//List of all python bindings
//----------------------- Add bindings ----------------------------------------------
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


