#pragma once


#include <cmath>
#include <cassert>
#include <cstring>
#include <stdio.h> 
#include <stdlib.h>

#include <map>
#include <list>
#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>
#include <unordered_map>
#include <condition_variable>


//Add Vector / Math classes

#define ULT_DEBUG 1

#if ULT_DEBUG 
#define ASSERT(cond,...) \
{	\
	if (!(cond)) { printf((__VA_ARGS__)); } \
	assert(cond);\
}
#else 
#define ASSERT(cond,...) 
#endif


struct Color
{
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct Rect
{
    int x, y;
    int w, h;
};

//note : order matches sdl2
enum Key
{
	KEY_UNKNOWN = '\0',
	KEY_RETURN = '\r',
	KEY_ESCAPE = '\033',
	KEY_BACKSPACE = '\b',
	KEY_TAB = '\t',
	KEY_SPACE = ' ',
	KEY_EXCLAIM = '!',
	KEY_QUOTEDBL = '"',
	KEY_HASH = '#',
	KEY_PERCENT = '%',
	KEY_DOLLAR = '$',
	KEY_AMPERSAND = '&',
	KEY_QUOTE = '\'',
	KEY_LEFTPAREN = '(',
	KEY_RIGHTPAREN = ')',
	KEY_ASTERISK = '*',
	KEY_PLUS = '+',
	KEY_COMMA = ',',
	KEY_MINUS = '-',
	KEY_PERIOD = '.',
	KEY_SLASH = '/',
	KEY_0 = '0',
	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',
	KEY_5 = '5',
	KEY_6 = '6',
	KEY_7 = '7',
	KEY_8 = '8',
	KEY_9 = '9',
	KEY_COLON = ':',
	KEY_SEMICOLON = ';',
	KEY_LESS = '<',
	KEY_EQUALS = '=',
	KEY_GREATER = '>',
	KEY_QUESTION = '?',
	KEY_AT = '@',
	KEY_LEFTBRACKET = '[',
	KEY_BACKSLASH = '\\',
	KEY_RIGHTBRACKET = ']',
	KEY_CARET = '^',
	KEY_UNDERSCORE = '_',
	KEY_BACKQUOTE = '`',
	KEY_a = 'a',
	KEY_b = 'b',
	KEY_c = 'c',
	KEY_d = 'd',
	KEY_e = 'e',
	KEY_f = 'f',
	KEY_g = 'g',
	KEY_h = 'h',
	KEY_i = 'i',
	KEY_j = 'j',
	KEY_k = 'k',
	KEY_l = 'l',
	KEY_m = 'm',
	KEY_n = 'n',
	KEY_o = 'o',
	KEY_p = 'p',
	KEY_q = 'q',
	KEY_r = 'r',
	KEY_s = 's',
	KEY_t = 't',
	KEY_u = 'u',
	KEY_v = 'v',
	KEY_w = 'w',
	KEY_x = 'x',
	KEY_y = 'y',
	KEY_z = 'z',
	KEY_UP = 'R',
	KEY_DOWN = 'Q',
	KEY_LEFT = 'P',
	KEY_RIGHT = 'O'
};


inline Key GetKey(char c)
{
	switch (c)
	{
	case KEY_RETURN:
	case KEY_ESCAPE:
	case KEY_BACKSPACE:
	case KEY_TAB :
	case KEY_SPACE:
	case KEY_EXCLAIM:
	case KEY_QUOTEDBL:
	case KEY_HASH:
	case KEY_PERCENT:
	case KEY_DOLLAR:
	case KEY_AMPERSAND:
	case KEY_QUOTE:
	case KEY_LEFTPAREN:
	case KEY_RIGHTPAREN:
	case KEY_ASTERISK:
	case KEY_PLUS:
	case KEY_COMMA:
	case KEY_MINUS:
	case KEY_PERIOD:
	case KEY_SLASH:
	case KEY_0:
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
	case KEY_COLON:
	case KEY_SEMICOLON:
	case KEY_LESS:
	case KEY_EQUALS:
	case KEY_GREATER:
	case KEY_QUESTION:
	case KEY_AT:
	case KEY_LEFTBRACKET:
	case KEY_BACKSLASH:
	case KEY_RIGHTBRACKET:
	case KEY_CARET:
	case KEY_UNDERSCORE:
	case KEY_BACKQUOTE:
	case KEY_a:
	case KEY_b:
	case KEY_c:
	case KEY_d:
	case KEY_e:
	case KEY_f:
	case KEY_g:
	case KEY_h:
	case KEY_i:
	case KEY_j:
	case KEY_k:
	case KEY_l:
	case KEY_m:
	case KEY_n:
	case KEY_o:
	case KEY_p:
	case KEY_q:
	case KEY_r:
	case KEY_s:
	case KEY_t:
	case KEY_u:
	case KEY_v:
	case KEY_w:
	case KEY_x:
	case KEY_y:
	case KEY_z:
	case KEY_UP:
	case KEY_DOWN:
	case KEY_LEFT:
	case KEY_RIGHT:
		return Key(c);
	default:
		return KEY_UNKNOWN;
	}
}