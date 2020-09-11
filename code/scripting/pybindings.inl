/*
Some notes

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


#define PYTHON_PRELUDE R"(
#layers
BG=0
SP=1
FG=2
UI=3
#button states
RELEASE = 0
CLICK = 1
HOLD = 2

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
PYDECL( idolon, scrw    , "Get screen width "                           )\
PYDECL( idolon, scrh    , "Get screen height "                          )\
PYDECL( idolon, mx      , "Get mouse x position"                        )\
PYDECL( idolon, my      , "Get mouse y position"                        )\
PYDECL( idolon, clear   , "Clear screen with color r,g,b"               )\
PYDECL( idolon, key     , "Get key state. 0 is up, 1 is down. 2 is hold")\
PYDECL( idolon, load    , "Load layer"                                  )\
PYDECL( idolon, unload  , "Unload layer"                                )\
PYDECL( idolon, view    , "Set the layer viewport"                      )\
PYDECL( idolon, scroll  , "Scroll layer by dx,dy"                       )\
PYDECL( idolon, sprite  , "Spawn sprite "                               )\
PYDECL( idolon, kill    , "Despawn sprite "                             )\
PYDECL( idolon, pos     , "Get or set sprite position"                  )\
PYDECL( idolon, move    , "Move sprite by x,y"                          )\
PYDECL( idolon, frame   , "Get or set sprite current tile"              )\
PYDECL( idolon, flip    , "Flip sprite tile by di"                      )\
PYDECL( idolon, sheet   , "Set surrent sprite sheetsprite "             )\


//-------------------------------------------------------------------//
PYBIND(idolon, scrw)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	return PyLong_FromLong(w);
}

//-------------------------------------------------------------------//
PYBIND(idolon, scrh)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	return PyLong_FromLong(h);
}

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
PYBIND( idolon, clear)
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
	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
PYBIND( idolon, key)
{
	
	Key key;
	if ( PyArg_ParseTuple( args, "i", &key ) == 0 )
	{
		PYERR( "Expected args (int)" );
		Py_RETURN_NONE;
	}
	printf( "KEY DOWN %d\n", Engine::GetKeyState( key ) );

	return PyLong_FromLong(Engine::GetKeyState( key ));
}
//-------------------------------------------------------------------//
PYBIND( idolon, load)
{
	
	int layer; 
	char *mapname;
	if ( PyArg_ParseTuple( args, "is", &layer, &mapname ) == 0 )
	{
		PYERR( "Expected args (int, string)" );
	}
	else
	{
		Runtime::Load( layer, mapname );
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
PYBIND( idolon, unload)
{
	
	int layer; 
	if ( PyArg_ParseTuple( args, "i", &layer)== 0 )
	{
		PYERR( "Expected args (int)" );
	}
	else
	{
		Runtime::Unload( layer );
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
PYBIND( idolon, view)
{
	
	int layer, x,y,w,h; 
	if ( PyArg_ParseTuple( args, "iiiii", &layer, &x, &y, &w, &h )== 0 )
	{
		PYERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::View( layer, x,y, w, h);
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
PYBIND( idolon, scroll)
{
	
	int layer, x,y; 
	if ( PyArg_ParseTuple( args, "iii", &layer, &x, &y)== 0 )
	{
		PYERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::Scroll( layer , x, y);
	}
	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
PYBIND( idolon, sprite )
{
	
	int id = -1;
	int tile,  x,  y,  isSmall = true;
	if ( PyArg_ParseTuple( args, "iii|i", &tile, &x, &y, &isSmall)== 0 )
	{
		PYERR( "Expected args (int, int, int, int?)" );
	}
	else
	{
		id = Runtime::Spawn( tile, x, y, isSmall );
	}

	return PyLong_FromLong(id);
}
//-------------------------------------------------------------------//
PYBIND( idolon, kill )
{
	
	int id; 
	if ( PyArg_ParseTuple( args, "i", &id)== 0 )
	{
		PYERR( "Expected args (int)" );
	}
	else
	{
		Runtime::Despawn( id );
	}

	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
PYBIND( idolon, pos )
{
	
	int id, x = -1,y = -1; 

	if ( PyArg_ParseTuple( args, "i|ii", &id,  &x, &y) == 0 )
	{
		PYERR( "Expected args (int, int?, int?)" );
	
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
PYBIND(idolon, move )
{
	
	int id, dx,dy; 
	if ( PyArg_ParseTuple( args, "iii", &id, &dx, &dy)== 0 )
	{
		PYERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::MoveBy( id, dx, dy);
	}

	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
PYBIND(idolon, frame )
{
	
	int id, tile; 
	
	if ( PyArg_ParseTuple( args, "i|ii", &id, &id, &tile) == 0 )
	{
		PYERR( "Expected args (int, int?)" );
		Py_RETURN_NONE;
	}
	
	Runtime::FlipTo( id, tile);
	return PyLong_FromLong(Runtime::Frame( id ));
}
//-------------------------------------------------------------------//
PYBIND(idolon, flip )
{	
	int id, di; 
	if ( PyArg_ParseTuple( args, "ii", &id, &di)== 0 )
	{
		PYERR( "Expected args (int, int)" );
	}
	else
	{
		Runtime::FlipBy( id, di);
	}

	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
PYBIND(idolon, sheet )
{
	char *sheet ;
	if ( PyArg_ParseTuple( args, "s", &sheet  ) == 0 )
	{
		PYERR( "Expected args (string)" );
	}
	else
	{
		Runtime::Sheet( sheet );
	}

	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//

