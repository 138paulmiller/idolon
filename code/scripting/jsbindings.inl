

#define DECL(module, name, doc )\
	{ #name, js_##module##_##name, doc } ,

#define BIND(module, name) \
	int js_##module##_##name(duk_context *ctx)


#include "prologue.inl"

#define LOG_MAX 2048

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
	return 1;
}

BIND( idolon, key    ) 
{
	return 1;
}

BIND( idolon, load   ) 
{
	return 1;
}

BIND( idolon, unload ) 
{
	return 1;
}

BIND( idolon, view   ) 
{
	return 1;
}

BIND( idolon, scroll ) 
{
	return 1;
}

BIND( idolon, sprite ) 
{
	return 1;
}

BIND( idolon, kill   ) 
{
	return 1;
}

BIND( idolon, pos    ) 
{
	return 1;
}

BIND( idolon, move   ) 
{
	return 1;
}

BIND( idolon, frame  ) 
{
	return 1;
}

BIND( idolon, flip   ) 
{
	return 1;
}

BIND( idolon, sheet  ) 
{
	return 1;
}


