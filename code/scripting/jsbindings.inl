
#define DECL(module, name, doc )\
	{ #name, js_##module##_##name, doc } ,

#define BIND(module, name) \
	int js_##module##_##name(duk_context *ctx)

//Note args are pushed in reverse order

#define ARG_STR(var) \
	if (duk_get_top(ctx) && duk_get_type_mask(ctx, -1) & DUK_TYPE_MASK_STRING)\
	{\
		var = duk_safe_to_string(ctx, -1);\
		duk_pop( ctx );\
	}\
	else\
	{\
		LOG("Script : Js : Expected string");\
		duk_pop( ctx );\
		return 0;\
	}



#define ARG_INT(var) \
	if (duk_get_top(ctx) && duk_get_type_mask(ctx, -1) & DUK_TYPE_MASK_NUMBER)\
	{\
		var = duk_to_int(ctx, -1);\
		duk_pop( ctx );\
	}\
	else\
	{\
		LOG("Script : Js : Expected integer");\
		duk_pop( ctx );\
		return 0;\
	}

#define RET() \
	return 1;

#define RET_INT(value) \
	duk_push_int(ctx, value);\
	return 1;

//Object return 

#define RET_BEG(obj, cnt) \
	duk_idx_t obj = duk_push_object(ctx);

#define RET_SET_INT(obj, id, value ) \
	duk_push_int(ctx, value);\
	duk_put_prop_string(ctx, obj, id);

#define RET_END(obj ) \
	return 1;

#include "bindings.inl"

//-------------------------------------------------------------------//
BIND(idolon, log)
{
	const int LOG_MAX = 2048;

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
