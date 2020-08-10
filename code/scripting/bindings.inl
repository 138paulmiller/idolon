#define DEF_PYTHON_BINDING(module, name)		PyObject* module##_##name(PyObject *self, PyObject *args)
#define ADD_PYTHON_BINDING(module, name,doc)	{#name, module##_##name, METH_VARARGS, doc},

//List of all python bindings
#define PYTHON_BINDINGS \
	ADD_PYTHON_BINDING(idolon, mouse, "Get mouse object")


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
