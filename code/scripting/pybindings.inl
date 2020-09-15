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

#include "prologue.inl"

//-------------------------------------------------------------------//
BIND(idolon, log)
{
	//todo log t
	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
BIND(idolon, scrw)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	return PyLong_FromLong(w);
}

//-------------------------------------------------------------------//
BIND(idolon, scrh)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	return PyLong_FromLong(h);
}

//-------------------------------------------------------------------//
BIND( idolon, mx)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	return PyLong_FromLong(x);
}

//-------------------------------------------------------------------//
BIND( idolon, my)
{
	
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	return PyLong_FromLong(y);
}

//-------------------------------------------------------------------//
BIND( idolon, clear)
{
	Color c = { 255,0,0,0 };
	if ( PyArg_ParseTuple( args, "iii", &c.r, &c.g, &c.b ) == 0 )
	{
		ERR( "Expected args (int, int, int)" );
	}
	else 
	{
		Engine::ClearScreen( c );
	}
	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
BIND( idolon, key)
{	
	Key key;
	if ( PyArg_ParseTuple( args, "i", &key ) == 0 )
	{
		ERR( "Expected args (int)" );
		Py_RETURN_NONE;
	}

	return PyLong_FromLong(Engine::GetKeyState( key ));
}
//-------------------------------------------------------------------//
BIND( idolon, load)
{
	
	int layer; 
	char *mapname;
	if ( PyArg_ParseTuple( args, "is", &layer, &mapname ) == 0 )
	{
		ERR( "Expected args (int, string)" );
	}
	else
	{
		Runtime::Load( layer, mapname );
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
BIND( idolon, unload)
{
	
	int layer; 
	if ( PyArg_ParseTuple( args, "i", &layer)== 0 )
	{
		ERR( "Expected args (int)" );
	}
	else
	{
		Runtime::Unload( layer );
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
BIND( idolon, view)
{
	
	int layer, x,y,w,h; 
	if ( PyArg_ParseTuple( args, "iiiii", &layer, &x, &y, &w, &h )== 0 )
	{
		ERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::View( layer, x,y, w, h);
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
BIND( idolon, scroll)
{
	
	int layer, x,y; 
	if ( PyArg_ParseTuple( args, "iii", &layer, &x, &y)== 0 )
	{
		ERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::Scroll( layer , x, y);
	}
	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
BIND( idolon, sprite )
{
	
	int id = -1;
	int tile,  x,  y,  isSmall = true;
	if ( PyArg_ParseTuple( args, "iii|i", &tile, &x, &y, &isSmall)== 0 )
	{
		ERR( "Expected args (int, int, int, int?)" );
	}
	else
	{
		id = Runtime::Spawn( tile, x, y, isSmall );
	}

	return PyLong_FromLong(id);
}
//-------------------------------------------------------------------//
BIND( idolon, kill )
{
	
	int id; 
	if ( PyArg_ParseTuple( args, "i", &id)== 0 )
	{
		ERR( "Expected args (int)" );
	}
	else
	{
		Runtime::Despawn( id );
	}

	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
BIND( idolon, pos )
{
	
	int id, x = -1,y = -1; 

	if ( PyArg_ParseTuple( args, "i|ii", &id,  &x, &y) == 0 )
	{
		ERR( "Expected args (int, int?, int?)" );
	
		Py_RETURN_NONE;
	}
	//get pos
	if ( x == -1 && y == -1 )
	{
		Runtime::Position( id, x, y );
	}

	Runtime::MoveTo( id, x, y);

	PyObject *xy = PyTuple_New(2);
	PyTuple_SetItem( xy, 0, PyLong_FromLong( x ) );
	PyTuple_SetItem( xy, 1, PyLong_FromLong( y ) );
	
	Py_XINCREF( xy );
	return xy;

}

//-------------------------------------------------------------------//
BIND(idolon, move )
{
	
	int id, dx,dy; 
	if ( PyArg_ParseTuple( args, "iii", &id, &dx, &dy)== 0 )
	{
		ERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::MoveBy( id, dx, dy);
	}

	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
BIND(idolon, frame )
{
	
	int id, tile = -1; 
	
	if ( PyArg_ParseTuple( args, "i|i", &id, &tile) == 0 )
	{
		ERR( "Expected args (int, int?)" );
		Py_RETURN_NONE;
	}
	if (tile != -1)
	{
		Runtime::FlipTo( id, tile);
	}
	return PyLong_FromLong(Runtime::Frame( id ));
}
//-------------------------------------------------------------------//
BIND(idolon, flip )
{	
	int id, di; 
	if ( PyArg_ParseTuple( args, "ii", &id, &di)== 0 )
	{
		ERR( "Expected args (int, int)" );
	}
	else
	{
		Runtime::FlipBy( id, di);
	}

	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
BIND(idolon, sheet )
{
	char *sheet ;
	if ( PyArg_ParseTuple( args, "s", &sheet  ) == 0 )
	{
		ERR( "Expected args (string)" );
	}
	else
	{
		Runtime::Sheet( sheet );
	}

	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//

