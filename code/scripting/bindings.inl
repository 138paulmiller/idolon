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

/*
	create prefix for object to operate on ? like spr_kill, spr_pos

*/
#define BINDINGS \
	DECL( idolon, log     , "Log message to debug console and file"    		)\
	DECL( idolon, require , "use code from system lib file"					)\
	DECL( idolon, key     , "Get key state. 0 is up, 1 is down. 2 is hold"	)\
	DECL( idolon, gfx_w   , "Get gfx width "								)\
	DECL( idolon, gfx_h   , "Get gfx height "								)\
	DECL( idolon, clear   , "Clear screen with color r,g,b"					)\
	DECL( idolon, mx      , "Get mouse x position"							)\
	DECL( idolon, my      , "Get mouse y position"							)\
	DECL( idolon, load    , "Load layer"									)\
	DECL( idolon, unload  , "Unload layer"									)\
	DECL( idolon, get_tile , "Get tile at given x,y"  						)\
	DECL( idolon, set_tile    , "Set tile at given x,y"  						)\
	DECL( idolon, view    , "Set the layer viewport"						)\
	DECL( idolon, scroll  , "Scroll layer by dx,dy"							)\
	DECL( idolon, sprite  , "Spawn sprite "									)\
	DECL( idolon, spr_w   , "Get sprite width "								)\
	DECL( idolon, spr_h   , "Get sprite height "								)\
	DECL( idolon, kill    , "Despawn sprite "								)\
	DECL( idolon, pos     , "Set sprite position"							)\
	DECL( idolon, move    , "Move sprite by x,y"							)\
	DECL( idolon, flip    , "Set sprite current tile"						)\
	DECL( idolon, sheet   , "Set surrent sprite sheetsprite "				)\


BIND( idolon, require )
{
	const char *scriptPath;
	
	ARG_STR(scriptPath);
	const int state = Idolon::Use( scriptPath );
	RET_INT( state != 0 );

}

BIND( idolon, key    ) 
{
	const char * keysym;
	ARG_STR(keysym);
	if ( keysym == nullptr ) 
	{
		RET_INT( 0 );
	}
	Key key = static_cast<Key>(*keysym);
	const int state = Engine::GetKeyState(key);
	RET_INT(state);
}

//-------------------------------------------------------------------//
BIND(idolon, gfx_w)
{
	static int w = 0;
	static int h = 0;
	Idolon::DisplaySize(w, h);
	RET_INT( w );
}

//-------------------------------------------------------------------//
BIND(idolon, gfx_h)
{
	static int w = 0;
	static int h = 0;
	Idolon::DisplaySize(w, h);
	RET_INT( h );

}

//-------------------------------------------------------------------//
BIND( idolon, mx)
{
	int x = 0;
	int y = 0;
	Idolon::MousePosition( x, y);
	RET_INT( x );

}

//-------------------------------------------------------------------//
BIND( idolon, my)
{
	int x = 0;
	int y = 0;
	Idolon::MousePosition( x, y);
	RET_INT( y );
}


BIND(idolon, clear)
{
	//get three args.
	int r, g, b;;
	ARG_INT(b);
	ARG_INT(g);
	ARG_INT(r);
	Idolon::Clear(r,g,b);
	RET();
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
BIND( idolon, get_tile )
{
	int layer, x,y;
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	int tile = Idolon::GetTile( layer, x, y );
	RET_INT(tile);
}

BIND( idolon, set_tile )
{

	int layer, x,y, tile;
	ARG_INT(tile);
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	Idolon::SetTile(layer, x,y, tile);
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


BIND( idolon, spr_w ) 
{
	int id = -1;
	int w, h;
	//TODO handle small sprite
	ARG_INT(id);

	id = Idolon::SpriteSize(id, w, h);

	RET_INT(w);
}


BIND( idolon, spr_h ) 
{
	int id = -1;
	int w, h;
	//TODO handle small sprite
	ARG_INT(id);

	id = Idolon::SpriteSize(id, w, h);

	RET_INT(h);
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

BIND( idolon, flip  ) 
{
	int id, tile;
	ARG_INT(tile);
	ARG_INT(id);

	Idolon::FlipTo(id, tile);
	int frame = Idolon::Frame( id );
	RET_INT( frame );
	
}


BIND( idolon, sheet  ) 
{
	const char *sheet;
	ARG_STR(sheet);
	
	Idolon::Sheet(sheet);
	RET();
}

