

#define DECL(module, name, doc )\
	{ #name, js_##module##_##name, doc } ,

#define BIND(module, name) \
	int js_##module##_##name(duk_context *ctx)


#include "prologue.inl"

#define LOG_MAX 2048

//Note args are pushed in reverse order

#define ARG_STR_OPT(var) \
	if (duk_get_top(ctx) && duk_get_type_mask(ctx, -1) & DUK_TYPE_MASK_STRING)\
	{\
		var = duk_safe_to_string(ctx, -1);\
		duk_pop( ctx );\
	}

#define ARG_STR(var) \
	ARG_STR_OPT(var)\
	else\
	{\
		LOG("Script : Js : Expected string");\
		return 0;\
	}

#define ARG_INT_OPT(var) \
	if (duk_get_top(ctx) && duk_get_type_mask(ctx, -1) & DUK_TYPE_MASK_NUMBER)\
	{\
		var = duk_to_int(ctx, -1);\
		duk_pop( ctx );\
	}

#define ARG_INT(var) \
	ARG_INT_OPT(var)\
	else\
	{\
		LOG("Script : Js : Expected integer");\
		return 0;\
	}

//-------------------------------------------------------------------//
BIND(idolon, log)
{
	
	char buffer[LOG_MAX];
	char *str = buffer;
	while ( duk_get_top( ctx ) )
	{
		if ( str - buffer > LOG_MAX ) break;
		if ( duk_get_type_mask( ctx, -1 ) & DUK_TYPE_MASK_STRING )
		{
			const char * strarg = duk_safe_to_string( ctx, -1 );
			str+=sprintf_s( str, LOG_MAX, "%s ", strarg );
		}
		else if (duk_get_type_mask( ctx, -1 ) & DUK_TYPE_MASK_NUMBER )
		{
			const int numarg = duk_to_number( ctx, -1 );
			str+=sprintf_s( str, LOG_MAX, "%d ", numarg );

		}
		else if (duk_get_type_mask( ctx, -1 ) & DUK_TYPE_MASK_BOOLEAN )
		{
			const int numarg = duk_to_boolean( ctx, -1 );
			str+=sprintf_s( str, LOG_MAX, "%d ", numarg );
		}
		else
		{
			ERR( "Js Script error: Invalid arg type" );
			break;
		}
		duk_pop( ctx );
	}

	LOG( buffer );
	LOG( "\n" );
	return 1;
}

//-------------------------------------------------------------------//
BIND(idolon, scrw)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	duk_push_int(ctx, w);
    return 1;
}

//-------------------------------------------------------------------//
BIND(idolon, scrh)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	duk_push_int(ctx, h);
    return 1;
}

//-------------------------------------------------------------------//
BIND( idolon, mx)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	duk_push_int(ctx, x);
    return 1;
}

//-------------------------------------------------------------------//
BIND( idolon, my)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	duk_push_int(ctx, y);
    return 1;
}


BIND(idolon, clear)
{
	//get three args.
	Color c = { 255,0,0,0 };
	ARG_INT(c.b);
	ARG_INT(c.g);
	ARG_INT(c.r);
	Engine::ClearScreen(c);

	return 1;
}

BIND( idolon, key    ) 
{
	int key;
	ARG_INT(key);

	duk_push_int(ctx, Engine::GetKeyState(static_cast<Key>(key)));
	return 1;
}

BIND( idolon, load   ) 
{	
	int layer;
	const char *mapname;
	int m = duk_get_type_mask(ctx, -1);
	
	ARG_STR(mapname);
	ARG_INT(layer);
	
	Runtime::Load(layer, mapname);

	return 1;
}

BIND( idolon, unload ) 
{
	int layer;
	
	ARG_INT(layer);

	Runtime::Unload(layer);

	return 1;
}

BIND( idolon, view   ) 
{
	int layer, x, y, w, h;
	
	ARG_INT(h);
	ARG_INT(w);
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);
	
	Runtime::View(layer, x, y, w, h);
	
	return 1;
}

BIND( idolon, scroll ) 
{
	int layer, x, y;
	
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(layer);

	Runtime::Scroll(layer, x, y);
	
	return 1;
}

BIND( idolon, sprite ) 
{
	int id = -1;
	int tile, x, y, isSmall = true;

	//TODO handle small sprite
	ARG_INT(y);
	ARG_INT(x);
	ARG_INT(tile);

	id = Runtime::Spawn(tile, x, y, isSmall);

	duk_push_int(ctx, id);
	return 1;
}

BIND( idolon, kill   ) 
{
	int id = -1;
	ARG_INT(id);

	Runtime::Despawn(id);

	return 1;
}

BIND( idolon, pos    ) 
{
	int id, x = -1, y = -1;
	ARG_INT_OPT(y);
	ARG_INT_OPT(x);
	ARG_INT(id);
	//get pos
	if (x == -1 && y == -1)
	{
		Runtime::Position(id, x, y);
	}

	Runtime::MoveTo(id, x, y);

	duk_idx_t obj = duk_push_object(ctx);
	duk_push_int(ctx, x);
	duk_put_prop_string(ctx, obj, "x");

	duk_push_int(ctx, y);
	duk_put_prop_string(ctx, obj, "y");
	
	return 1;
}

BIND( idolon, move   ) 
{
	int id, dx, dy;
	ARG_INT(dy);
	ARG_INT(dx);
	ARG_INT(id);
	
	Runtime::MoveBy(id, dx, dy);

	return 1;
}

BIND( idolon, frame  ) 
{
	int id, tile = - 1;
	ARG_INT_OPT(tile);
	ARG_INT(id);

	if (tile != -1)
	{
		Runtime::FlipTo(id, tile);
	}
	
	duk_push_int(ctx, Runtime::Frame(id));
	
	return 1;
}

BIND( idolon, flip   ) 
{
	int id, di;
	ARG_INT(di);
	ARG_INT(id);

	Runtime::FlipBy(id, di);

	return 1;
}

BIND( idolon, sheet  ) 
{
	const char *sheet;
	ARG_STR(sheet);
	
	Runtime::Sheet(sheet);

	return 1;
}


