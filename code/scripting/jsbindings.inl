#define JS_PRELUDE R"(

MAP_BG=0
MAP_SP=1
MAP_FG=2
MAP_UI=3

BTN_RELEASE = 0
BTN_CLICK = 1
BTN_HOLD = 2

KEY_UNKNOWN = '\0'
KEY_RETURN = '\r'
KEY_ESCAPE = '\033'
KEY_BACKSPACE = '\b'
KEY_TAB = '\t'
KEY_SPACE = ' '
KEY_EXCLAIM = '!'
KEY_QUOTEDBL = '"'
KEY_HASH = '#'
KEY_PERCENT = '%'
KEY_DOLLAR = '$'
KEY_AMPERSAND = '&'
KEY_QUOTE = '\''
KEY_LEFTPAREN = '('
KEY_RIGHTPAREN = ')'
KEY_ASTERISK = '*'
KEY_PLUS = '+'
KEY_COMMA = ''
KEY_MINUS = '-'
KEY_PERIOD = '.'
KEY_SLASH = '/'
KEY_0 = '0'
KEY_1 = '1'
KEY_2 = '2'
KEY_3 = '3'
KEY_4 = '4'
KEY_5 = '5'
KEY_6 = '6'
KEY_7 = '7'
KEY_8 = '8'
KEY_9 = '9'
KEY_COLON = ':'
KEY_SEMICOLON = ';'
KEY_LESS = '<'
KEY_EQUALS = '='
KEY_GREATER = '>'
KEY_QUESTION = '?'
KEY_AT = '@'
KEY_A = 'A'
KEY_B = 'B'
KEY_C = 'C'
KEY_D = 'D'
KEY_E = 'E'
KEY_F = 'F'
KEY_G = 'G'
KEY_H = 'H'
KEY_I = 'I'
KEY_J = 'J'
KEY_K = 'K'
KEY_L = 'L'
KEY_M = 'M'
KEY_N = 'N'
KEY_O = 'O'
KEY_P = 'P'
KEY_Q = 'Q'
KEY_R = 'R'
KEY_S = 'S'
KEY_T = 'T'
KEY_U = 'U'
KEY_V = 'V'
KEY_W = 'W'
KEY_X = 'X'
KEY_Y = 'Y'
KEY_Z = 'Z'
KEY_LEFTBRACKET = '['
KEY_BACKSLASH = '\\'
KEY_RIGHTBRACKET = ']'
KEY_CARET = '^'
KEY_UNDERSCORE = '_'
KEY_BACKQUOTE = '`'
KEY_a = 'a'
KEY_b = 'b'
KEY_c = 'c'
KEY_d = 'd'
KEY_e = 'e'
KEY_f = 'f'
KEY_g = 'g'
KEY_h = 'h'
KEY_i = 'i'
KEY_j = 'j'
KEY_k = 'k'
KEY_l = 'l'
KEY_m = 'm'
KEY_n = 'n'
KEY_o = 'o'
KEY_p = 'p'
KEY_q = 'q'
KEY_r = 'r'
KEY_s = 's'
KEY_t = 't'
KEY_u = 'u'
KEY_v = 'v'
KEY_w = 'w'
KEY_x = 'x'
KEY_y = 'y'
KEY_z = 'z'
KEY_LEFTBRACE = '{'
KEY_BAR = '|'
KEY_RIGHTBRACE = '}'
KEY_TILDA = '~'
KEY_ESCAPE = '\033'
KEY_SHIFT = 127
KEY_ALT = SHIFT + 1
KEY_UP = ALT + 1
KEY_DOWN = UP + 1
KEY_LEFT = DOWN + 1
KEY_RIGHT = LEFT + 1


exp = Math.exp
sin = Math.sin
cos = Math.cos
tan = Math.tan
pow = Math.pow


function clamp(x, min, max){
	if (x < min) return min
	if (x > max) return max
	return x

}
function vec2(x, y){

    this.x = x
    this.y = y
    
    this.add = function( v ){
		return new vec2(this.x + v.x, this.y + v.y)
    }


    this.sub = function( v ){
		return new vec2(this.x + v.x, this.y + v.y)
    }

    
    this.dot = function( v ){
        return this.x * v.x +  this.y * v.y
    }
    
    this.len  = function( ){
        return this.dot(this)
   }
}

)"

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
