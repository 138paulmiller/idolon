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


#define PYTHON_DEFINES R"(
#layers
BG=0
SP=1
FG=2
UI=3
#button states
CLICK = 1
RELEASE = 0
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
	PYDECL( idolon, mx      , "Get mouse x position"                        )\
	PYDECL( idolon, my      , "Get mouse y position"                        )\
	PYDECL( idolon, cls     , "Clear screen with color r,g,b"               )\
	PYDECL( idolon, key     , "Get key state. 0 is up, 1 is down. 2 is hold")\
	PYDECL( idolon, load    , "Load layer"                                  )\
	PYDECL( idolon, unload  , "Unload layer"                                )\
	PYDECL( idolon, resize  , "Resize layer"                                )\
	PYDECL( idolon, scroll  , "Scroll layer"                                )\
	PYDECL( idolon, spawn   , "Spawn sprite "                               )\
	PYDECL( idolon, despawn , "Despawn sprite "                             )\
	PYDECL( idolon, moveTo  , "Move sprite to x,y "                         )\
	PYDECL( idolon, moveBy  , "Move sprite by x,y"                          )\
	PYDECL( idolon, flipTo  , "Move sprite tile to id"                      )\
	PYDECL( idolon, flipBy  , "Move sprite tile by id"                      )\
	PYDECL( idolon, sheet   , "Set surrent sprite sheetsprite "             )\



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
PYBIND( idolon, resize)
{
	int layer, w,h; 
	if ( PyArg_ParseTuple( args, "iii", &layer, &w, &h )== 0 )
	{
		PYERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::Resize( layer , w, h);
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
PYBIND( idolon, spawn )
{
	int id = -1;
	int tile,  x,  y,  isSmall ;
	if ( PyArg_ParseTuple( args, "iii", &tile, &x, &y)== 0 )
	{
		PYERR( "Expected args (int, int, int, int?)" );
	}
	else
	{
		if ( PyArg_ParseTuple( args, "i", &isSmall ) == 0 )
		{
			isSmall = true;
		}
		id = Runtime::Spawn( tile, x, y, isSmall );
	}
	return PyLong_FromLong(id);
}
//-------------------------------------------------------------------//
PYBIND( idolon, despawn)
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
PYBIND( idolon, moveTo )
{
	int id, x,y; 
	if ( PyArg_ParseTuple( args, "iii", &id, &x, &y)== 0 )
	{
		PYERR( "Expected args (int, int, int)" );
	}
	else
	{
		Runtime::MoveTo( id, x, y);
	}
	Py_RETURN_NONE;
}

//-------------------------------------------------------------------//
PYBIND(idolon, moveBy)
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
PYBIND(idolon, flipTo)
{
	int id, tile; 
	if ( PyArg_ParseTuple( args, "ii", &id, &tile)== 0 )
	{
		PYERR( "Expected args (int, int)" );
	}
	else
	{
		Runtime::FlipTo( id, tile);
	}
	Py_RETURN_NONE;
}
//-------------------------------------------------------------------//
PYBIND(idolon, flipBy)
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

