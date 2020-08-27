#pragma once

#ifdef __unix__         
#define OS_LINUX
#elif defined(_WIN32) || defined(_WIN64)  
#define OS_WINDOWS
#endif

#include "err.hpp"
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <sstream>


//todo mov to config
#define TAB_SIZE 4

#define LOG(...) printf(__VA_ARGS__);


typedef unsigned char uint8;
typedef unsigned int uint;


//Add Vector / Math classes
#define Max( x, y ) (((x) > (y)) ? x : y)

std::string Trim( const std::string &s );

struct Color
{
    uint8 a;
    uint8 r;
    uint8 g;
    uint8 b;
	Color( uint8 a = 0, uint8 r= 0, uint8 g= 0, uint8 b = 0);
	bool operator==(const Color & c) const;
    bool operator!=(const Color & c) const;
};


extern const Color Palette[];
extern const int PaletteCount;

//clear as in transparent, not the color used to redraw the display
#define CLEAR Color()
#define BLACK Palette[0]
#define WHITE Palette[1]
#define HIGHLIGHT Color( 255 / 2, 250, 250, 250 )

//the rate in which cursors flicker on and off
#define CURSOR_FLICKER_RATE  3

struct Rect
{
    int x, y;
    int w, h;
	bool intersects(const Rect & other) const;
};

//Each layer is a tileset in memory
enum Layer
{
	//In Draw Order
	LAYER_BG=0, //background layer
	LAYER_SP, //sprite layer
	LAYER_FG, //foregroud layer
	LAYER_UI, //user-interface layer
	LAYER_COUNT
};


//note : order matches sdl2 keycodes
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
	KEY_A = 'A',
	KEY_B = 'B',
	KEY_C = 'C',
	KEY_D = 'D',
	KEY_E = 'E',
	KEY_F = 'F',
	KEY_G = 'G',
	KEY_H = 'H',
	KEY_I = 'I',
	KEY_J = 'J',
	KEY_K = 'K',
	KEY_L = 'L',
	KEY_M = 'M',
	KEY_N = 'N',
	KEY_O = 'O',
	KEY_P = 'P',
	KEY_Q = 'Q',
	KEY_R = 'R',
	KEY_S = 'S',
	KEY_T = 'T',
	KEY_U = 'U',
	KEY_V = 'V',
	KEY_W = 'W',
	KEY_X = 'X',
	KEY_Y = 'Y',
	KEY_Z = 'Z',
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
	KEY_LEFTBRACE = '{',
	KEY_BAR = '|',
	KEY_RIGHTBRACE = '}',
	KEY_TILDA = '~',
	//non-symbol related
	KEY_SHIFT = KEY_TILDA + 1,
	KEY_ALT,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
};

inline bool KeyPrintable(const Key key)
{
	return key >= KEY_TAB && key <= KEY_TILDA;
}
