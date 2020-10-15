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
	DECL( idolon, Log     , "Log message to debug console and file"    		)\
	DECL( idolon, Require , "use code from system lib file"					)\
	DECL( idolon, Load   , "Load asset"									)\
	DECL( idolon, Unload  , "Unload asset"									)\
	DECL( idolon, GfxSize   , "Get gfx width height {w,h}"								)\
	DECL( idolon, GfxMode, "Set draw mode "								)\
	DECL( idolon, GfxClear   , "Clear screen with color r,g,b"					)\
	DECL( idolon, KeyState     , "Get key state. 0 is up, 1 is down. 2 is hold"	)\
	DECL( idolon, MousePos , "Get mouse position {x,y}"							)\
	DECL( idolon, MapLayer    , "Load layer"									)\
	DECL( idolon, GetTile , "Get tile at given x,y"  						)\
	DECL( idolon, SetTile    , "Set tile at given x,y"  						)\
	DECL( idolon, MapView    , "Set the layer viewport"						)\
	DECL( idolon, MapScroll  , "Scroll layer by dx,dy"							)\
	DECL( idolon, SprSpawn, "Spawn sprite "									)\
	DECL( idolon, SprKill    , "Despawn sprite "								)\
	DECL( idolon, SprSize   , "Get sprite width height {w,h} "								)\
	DECL( idolon, SprPos     , "Set sprite position"							)\
	DECL( idolon, SprFlip    , "Set sprite current tile"						)\
	DECL( idolon, SprSheet   , "Set surrent sprite sheetsprite "				)\



BIND(idolon, Log)
{
	ARGS_VARLIST( 
		LOG( "%i ", i ),
		LOG( "%f ", f ),
		LOG( "%s ", s )
	)
	LOG( "\n" );
	RET();
}

BIND( idolon, Require )
{
	const char *scriptPath;
	
	ARG_STR(scriptPath);
	const int state = Idolon::Import( scriptPath );
	RET_INT( state != 0 );

}

BIND( idolon, Load )
{
	const char *assetName;
	ARG_STR(assetName);
	const int state = Idolon::Load( assetName );
	RET_INT( state != 0 );
}


BIND( idolon, Unload )
{
	int assetId;
	ARG_INT(assetId);
	Idolon::Unload( assetId );
	RET( );
}

BIND( idolon, KeyState    ) 
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
BIND(idolon, GfxSize )
{
	static int w = 0;
	static int h = 0;
	Idolon::GfxSize(w, h);
	RET_BEG( obj, 2 );
	RET_SET_INT( obj, "w", w );
	RET_SET_INT( obj, "h", h );
	RET_END( obj );
}



//-------------------------------------------------------------------//
BIND( idolon, MousePos )
{
	int x = 0;
	int y = 0;
	Idolon::MousePosition( x, y);
	RET_BEG( obj, 2 );
	RET_SET_INT( obj, "x", x );
	RET_SET_INT( obj, "y", y );
	RET_END( obj );

}


BIND(idolon, GfxMode )
{
	//get three args.
	int mode;
	ARG_INT(mode);
	Idolon::GfxDrawMode(mode);
	RET();
}


BIND(idolon, GfxClear )
{
	//get three args.
	int r, g, b;;
	ARG_INT(b);
	ARG_INT(g);
	ARG_INT(r);
	Idolon::GfxClear(r,g,b);
	RET();
}

BIND( idolon, MapLayer   ) 
{	
	int layer,mapId;
	
	ARG_INT(mapId);
	ARG_INT(layer);
	
	Idolon::MapLoad(layer, mapId);
	RET();
}


BIND( idolon, GetTile )
{
	int layer, x,y;
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	int tile = Idolon::MapGetTile( layer, x, y );
	RET_INT(tile);
}

BIND( idolon, SetTile )
{

	int layer, x,y, tile;
	ARG_INT(tile);
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	Idolon::MapSetTile(layer, x,y, tile);
	RET();
}

BIND( idolon, MapView   ) 
{
	int layer, x, y, w, h;
	
	ARG_INT(h);
	ARG_INT(w);
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	
	Idolon::MapView(layer, x, y, w, h);
	RET();
}

BIND( idolon, MapScroll ) 
{
	int layer, x, y;
	
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);

	Idolon::MapScrollTo(layer, x, y);
	RET();
}

BIND( idolon, SprSpawn ) 
{
	int id = -1;
	int tile, x, y, isSmall = true;

	//TODO handle small sprite
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(tile);

	id = Idolon::SprSpawn(tile, x, y, isSmall);

	RET_INT(id);
}


BIND( idolon, SprSize ) 
{
	int id = -1;
	int w, h;
	//TODO handle small sprite
	ARG_INT(id);

	id = Idolon::SprGetSize(id, w, h);
	RET_BEG( obj, 2 );
	RET_SET_INT( obj, "w", w );
	RET_SET_INT( obj, "h", h );
	RET_END( obj );
}


BIND( idolon, SprKill   ) 
{
	int id = -1;
	ARG_INT(id);

	Idolon::SprDespawn(id);

	RET();
}

BIND( idolon, SprPos ) 
{
	int id, x = -1, y = -1;
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(id);

	Idolon::SprSetPosition(id, x, y);

	RET_BEG( obj, 2 );
	RET_SET_INT( obj, "x", x );
	RET_SET_INT( obj, "y", y );
	RET_END( obj );
}


BIND( idolon, SprFlip  ) 
{
	int id, tile;
	ARG_INT(tile);
	ARG_INT(id);
	Idolon::SprSetFrame(id, tile);
	RET( );
	
}


BIND( idolon, SprSheet  ) 
{
	int sheetId;
	ARG_INT(sheetId);
	
	Idolon::SprSetSheet(sheetId);
	RET();
}

