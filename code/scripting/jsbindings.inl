
#define JS_PRELUDE R"(
//layers
BG=0
SP=1
FG=2
UI=3

//button states
RELEASE = 0
CLICK = 1
HOLD = 2

//keys
ESCAPE = '\033'
SHIFT = 127
ALT = SHIFT + 1
UP = ALT + 1
DOWN = UP + 1
LEFT = DOWN + 1
RIGHT = LEFT + 1

)"


#define JS_BINDINGS \
	JSDECL( idolon, log		, "Log message to debug console and file"       )\
	JSDECL( idolon, scrw    , "Get screen width "                           )\
	JSDECL( idolon, scrh    , "Get screen height "                          )\
	JSDECL( idolon, mx      , "Get mouse x position"                        )\
	JSDECL( idolon, my      , "Get mouse y position"                        )\


#define LOG_MAX 2048

//-------------------------------------------------------------------//
JSBIND(idolon, log)
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
			JSERR( "Js Script error: Invalid arg type" );
			break;
		}
		duk_pop( ctx );
	}

	LOG( buffer );
	LOG( "\n" );
	return 1;
}

//-------------------------------------------------------------------//
JSBIND(idolon, scrw)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	duk_push_int(ctx, w);
    return 1;
}

//-------------------------------------------------------------------//
JSBIND(idolon, scrh)
{
	static int w = 0;
	static int h = 0;
	Engine::GetSize(w, h);
	duk_push_int(ctx, h);
    return 1;
}

//-------------------------------------------------------------------//
JSBIND( idolon, mx)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	duk_push_int(ctx, x);
    return 1;
}

//-------------------------------------------------------------------//
JSBIND( idolon, my)
{
	static int x = 0;
	static int y = 0;
	Engine::GetMousePosition( x, y);
	duk_push_int(ctx, y);
    return 1;
}