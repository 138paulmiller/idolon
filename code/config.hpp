#pragma once
//TODO create settings manager t oload this from a cfg file

//Default settings wrap with ifndef to allow config ? 
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define SYSTEM_NAME "idolon"

#define WINDOW_SCALE 2

//in virtual pixel space
#define SCREEN_W 260
#define SCREEN_H 256


#define FPS_STAT_RATE  5.f 
//the rate in which cursors flicker on and off
#define CURSOR_FLICKER_RATE  3
#define GAME_API_UPDATE "OnFrame"
#define GAME_API_ONKEY "OnKey"

#define SHELL_PREFIX "$"
#define DEFAULT_FONT "default"
#define DEFAULT_FONT_ICONS "icons" //see s_iconNameMap for usage
#define DEFAULT_TEXT_BORDER 2
#define DEFAULT_FONT_EXT ".fnt"
#define DEFAULT_MAP_EXT ".map" 
#define DEFAULT_TILESET_EXT ".tls"
#define DEFAULT_SCRIPT_EXT ".scr"
#define DEFAULT_GAME_HEADER_EXT  ".cfg"
#define DEFAULT_CARTRIDGE_EXT  ".cart"
#define DEFAULT_COLOR_TEXT  WHITE
#define DEFAULT_COLOR_FILL  BLACK
#define DEFAULT_COLOR_CLICK Palette[26] 
#define DEFAULT_COLOR_HOVER Palette[25]

//editor "background" color. 
#define EDITOR_COLOR Palette[17]
#define CURSOR_COLOR Palette[25]
#define BORDER_COLOR WHITE


//clear as in transparent, not the color used to redraw the display
#define CLEAR { 0, 0, 0, 0 }
#define BLACK { 255, 0, 0, 0 }
#define WHITE { 255, 255, 255, 255 }
#define HIGHLIGHT { 255 / 2, 250, 250, 250 }


#define ARG_NAME_TILESET "tls"
#define ARG_NAME_FONT "font"
#define ARG_NAME_MAP "map"
#define ARG_NAME_SCRIPT "scr"
#define ARG_NAME_GAME "game"

//remove this 
#define MAX_TILESET_COUNT 4
#define MAX_MAP_COUNT     4
#define MAX_SCRIPT_COUNT  4




//Default Tileset sizes
#define SPRITE_W 16
#define SPRITE_H 16
#define SPRITE_W_SMALL 8
#define SPRITE_H_SMALL 8
#define SPRITE_COUNT (TILESET_W/SPRITE_W * TILESET_H/SPRITE_W) 
#define SPRITE_SMALL_COUNT (TILESET_W/SPRITE_W_SMALL * TILESET_H/SPRITE_H_SMALL) 


#define MAP_W 128
#define MAP_H 128

//Max number of tilesets per map
#define TILESETS_PER_MAP 2

//Default Tileset sizes. Do not change!
//Size fits to bottom of the screen
#define TILESET_W 256
#define TILESET_H 64
//Default Tileset sizes
#define TILE_W 8
#define TILE_H 8
//256 tiles, char indexed
#define TILE_COUNT (TILESET_W/TILE_W * TILESET_H/TILE_H) 

//if tile index is -1, then will be clear
#define TILE_CLEAR -1
