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
	DECL( idolon, load   , "Load asset"									)\
	DECL( idolon, unload  , "Unload asset"									)\
	DECL( idolon, key     , "Get key state. 0 is up, 1 is down. 2 is hold"	)\
	DECL( idolon, gfx_w   , "Get gfx width "								)\
	DECL( idolon, gfx_h   , "Get gfx height "								)\
	DECL( idolon, clear   , "Clear screen with color r,g,b"					)\
	DECL( idolon, mx      , "Get mouse x position"							)\
	DECL( idolon, my      , "Get mouse y position"							)\
	DECL( idolon, layer    , "Load layer"									)\
	DECL( idolon, get_tile , "Get tile at given x,y"  						)\
	DECL( idolon, set_tile    , "Set tile at given x,y"  						)\
	DECL( idolon, view    , "Set the layer viewport"						)\
	DECL( idolon, scroll  , "Scroll layer by dx,dy"							)\
	DECL( idolon, spr, "Spawn sprite "									)\
	DECL( idolon, spr_w   , "Get sprite width "								)\
	DECL( idolon, spr_h   , "Get sprite height "								)\
	DECL( idolon, spr_kill    , "Despawn sprite "								)\
	DECL( idolon, spr_pos     , "Set sprite position"							)\
	DECL( idolon, spr_move    , "Move sprite by x,y"							)\
	DECL( idolon, spr_flip    , "Set sprite current tile"						)\
	DECL( idolon, spr_sheet   , "Set surrent sprite sheetsprite "				)\


BIND( idolon, require )
{
	const char *scriptPath;
	
	ARG_STR(scriptPath);
	const int state = Idolon::Import( scriptPath );
	RET_INT( state != 0 );

}

BIND( idolon, load )
{
	const char *assetName;
	ARG_STR(assetName);
	const int state = Idolon::Load( assetName );
	RET_INT( state != 0 );
}


BIND( idolon, unload )
{
	int assetId;
	ARG_INT(assetId);
	Idolon::Unload( assetId );
	RET( );
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
	Idolon::GfxSize(w, h);
	RET_INT( w );
}

//-------------------------------------------------------------------//
BIND(idolon, gfx_h)
{
	static int w = 0;
	static int h = 0;
	Idolon::GfxSize(w, h);
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
	Idolon::GfxClear(r,g,b);
	RET();
}

BIND( idolon, layer   ) 
{	
	int layer,mapId;
	
	ARG_INT(mapId);
	ARG_INT(layer);
	
	Idolon::SetLayer(layer, mapId);
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
	
	Idolon::SetView(layer, x, y, w, h);
	RET();
}

BIND( idolon, scroll ) 
{
	int layer, x, y;
	
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);

	Idolon::ScrollTo(layer, x, y);
	RET();
}

BIND( idolon, spr ) 
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

	id = Idolon::GetSpriteSize(id, w, h);

	RET_INT(w);
}


BIND( idolon, spr_h ) 
{
	int id = -1;
	int w, h;
	//TODO handle small sprite
	ARG_INT(id);

	id = Idolon::GetSpriteSize(id, w, h);

	RET_INT(h);
}

BIND( idolon, spr_kill   ) 
{
	int id = -1;
	ARG_INT(id);

	Idolon::Despawn(id);

	RET();
}

BIND( idolon, spr_pos    ) 
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

BIND( idolon, spr_move   ) 
{
	int id, dx, dy;
	ARG_INT(dy);
	ARG_INT(dx);
	ARG_INT(id);
	
	Idolon::MoveBy(id, dx, dy);
	RET( );

}

BIND( idolon, spr_flip  ) 
{
	int id, tile;
	ARG_INT(tile);
	ARG_INT(id);

	Idolon::FlipTo(id, tile);
	int frame = Idolon::GetFrame( id );
	RET_INT( frame );
	
}


BIND( idolon, spr_sheet  ) 
{
	int sheetId;
	ARG_INT(sheetId);
	
	Idolon::SetSpriteSheet(sheetId);
	RET();
}

