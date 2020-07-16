#pragma once

#ifdef __unix__         
#define OS_LINUX
#elif defined(_WIN32) || defined(_WIN64)  
#define OS_WINDOWS
#endif


#include "pch.h"
#include "err.h"


//Default settings

#define WINDOW_TITLE "idolon"
#define WINDOW_SCALE 1.0/2
//in virtual pixel space. not window size
#define SCREEN_W 256
#define SCREEN_H 256
//TODO make font 8x8. need to edit sheet and then reconvert

#define FONT_DEFAULT "basic"
//Default Tileset size
#define TILESET_W 64
#define TILESET_H 64
//Default Tileset sizes
#define TILE_W 16
#define TILE_H 16
#define TILE_W_SMALL 8
#define TILE_H_SMALL 8

//Add Vector / Math classes


struct Color
{
    uint8_t a;
    uint8_t r;
    uint8_t g;
    uint8_t b;
	Color( uint8_t a = 0, uint8_t r= 0, uint8_t g= 0, uint8_t b = 0);
	bool operator==(const Color & c) const;
    bool operator!=(const Color & c) const;
};

#define BLACK Palette[0]
#define WHITE Palette[1]
#define CLEAR Color()
#define HIGHLIGHT Color( 255 / 2, 255, 255, 255 )
#define CURSOR_COLOR Palette[25]
#define BORDER_COLOR WHITE
//editor "background" color. This color should not be selectable
#define EDITOR_COLOR Palette[17]

//Clear color
extern const Color Palette[];
extern const int PaletteCount;


struct Rect
{
    int x, y;
    int w, h;
	bool intersects(const Rect & other) const;
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
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT
};
