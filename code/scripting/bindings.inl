//API that must be defined in order to support language bindings
//#define DECL(module, name, doc )
//#define BIND(module, name) 
//#define ARG_STR_OPT(var)
//#define ARG_STR(var) 
//#define ARG_INT_OPT(var) 
//#define ARG_INT(var) 
//#define RET() 
//#define RET_INT(value) 
//#define RET_BEG(obj,cnt) 
//#define RET_SET_INT(obj, id, value ) 
//#define RET_END( obj )


//Arguments are called in order determined by popping call stack (reversed)!

#define ERR(...) \
	LOG(__VA_ARGS__)

#define BINDINGS_CONSTS  R"(
BG=0
SP=1
FG=2
UI=3
RELEASE = 0
CLICK = 1
HOLD = 2
ESCAPE = '\033'
SHIFT = 127
ALT = SHIFT + 1
UP = ALT + 1
DOWN = UP + 1
LEFT = DOWN + 1
RIGHT = LEFT + 1
)"


#define BINDINGS \
	DECL( idolon, log     , "Log message to debug console and file"    	)\
	DECL( idolon, scrw    , "Get screen width "                           )\
	DECL( idolon, scrh    , "Get screen height "                          )\
	DECL( idolon, mx      , "Get mouse x position"                        )\
	DECL( idolon, my      , "Get mouse y position"                        )\
	DECL( idolon, clear   , "Clear screen with color r,g,b"               )\
	DECL( idolon, key     , "Get key state. 0 is up, 1 is down. 2 is hold")\
	DECL( idolon, load    , "Load layer"                                  )\
	DECL( idolon, unload  , "Unload layer"                                )\
	DECL( idolon, view    , "Set the layer viewport"                      )\
	DECL( idolon, scroll  , "Scroll layer by dx,dy"                       )\
	DECL( idolon, sprite  , "Spawn sprite "                               )\
	DECL( idolon, kill    , "Despawn sprite "                             )\
	DECL( idolon, pos     , "Set sprite position"                  )\
	DECL( idolon, move    , "Move sprite by x,y"                          )\
	DECL( idolon, frame   , "Set sprite current tile"              )\
	DECL( idolon, flip    , "Flip sprite tile by di"                      )\
	DECL( idolon, sheet   , "Set surrent sprite sheetsprite "             )\





//-------------------------------------------------------------------//
BIND(idolon, scrw)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	RET_INT( w );
}

//-------------------------------------------------------------------//
BIND(idolon, scrh)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	RET_INT( h );

}

//-------------------------------------------------------------------//
BIND( idolon, mx)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	RET_INT( x );

}

//-------------------------------------------------------------------//
BIND( idolon, my)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	RET_INT( y );
}


BIND(idolon, clear)
{
	//get three args.
	Color c = { 255,0,0,0 };
	ARG_INT(c.b);
	ARG_INT(c.g);
	ARG_INT(c.r);
	Engine::ClearScreen(c);
	RET();
}

BIND( idolon, key    ) 
{
	int key;
	ARG_INT(key);
	const int state = Engine::GetKeyState( static_cast<Key>( key ) );
	RET_INT(state);
}

BIND( idolon, load   ) 
{	
	int layer;
	const char *mapname;
	
	ARG_STR(mapname);
	ARG_INT(layer);
	
	Idolon::Load(layer, mapname);
	RET();
}

BIND( idolon, unload ) 
{
	int layer;
	
	ARG_INT(layer);
	Idolon::Unload(layer);
	RET();
}

BIND( idolon, view   ) 
{
	int layer, x, y, w, h;
	
	ARG_INT(h);
	ARG_INT(w);
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	
	Idolon::View(layer, x, y, w, h);
	RET();
}

BIND( idolon, scroll ) 
{
	int layer, x, y;
	
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);

	Idolon::Scroll(layer, x, y);
	RET();
}

BIND( idolon, sprite ) 
{
	int id = -1;
	int tile, x, y, isSmall = true;

	//TODO handle small sprite
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(tile);

	id = Idolon::Spawn(tile, x, y, isSmall);

	RET_INT(id);
}

BIND( idolon, kill   ) 
{
	int id = -1;
	ARG_INT(id);

	Idolon::Despawn(id);

	RET();
}

BIND( idolon, pos    ) 
{
	int id, x = -1, y = -1;
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(id);
	//get pos
	if (x == -1 && y == -1)
	{
		Idolon::Position(id, x, y);
	}

	Idolon::MoveTo(id, x, y);

	RET_BEG( obj, 2 );
	RET_SET_INT( obj, "x", x );
	RET_SET_INT( obj, "y", y );
	RET_END( obj );
}

BIND( idolon, move   ) 
{
	int id, dx, dy;
	ARG_INT(dy);
	ARG_INT(dx);
	ARG_INT(id);
	
	Idolon::MoveBy(id, dx, dy);
	RET( );

}

BIND( idolon, frame  ) 
{
	int id, tile;
	ARG_INT(tile);
	ARG_INT(id);

	if (tile != -1)
	{
		Idolon::FlipTo(id, tile);
	}

	RET_INT( Idolon::Frame(id));
	
}

BIND( idolon, flip   ) 
{
	int id, di;
	ARG_INT(di);
	ARG_INT(id);

	Idolon::FlipBy(id, di);

	RET();
}

BIND( idolon, sheet  ) 
{
	const char *sheet;
	ARG_STR(sheet);
	
	Idolon::Sheet(sheet);
	RET();
}


