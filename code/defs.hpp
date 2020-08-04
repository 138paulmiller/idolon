#pragma once
//Default settings wrap with ifndef to allow config ? 
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define SYSTEM_NAME "idolon"

#define WINDOW_SCALE 2
//in virtual pixel space. not window size
#define SCREEN_W 256
#define SCREEN_H 256
//TODO make font 8x8. need to edit sheet and then reconvert
#define SHELL_PREFIX ">"
#define DEFAULT_FONT "default"
//Default Tileset size
//Size fits to bottom of the screen
#define TILESET_W 128
#define TILESET_H 32
//Default Tileset sizes
#define TILE_W 8
#define TILE_H 8
#define TILE_COUNT (TILESET_W/TILE_W * TILESET_H/TILE_H) 

#define MAP_W 128
#define MAP_H 128
//if tile index is -1, then will be clear
#define TILE_CLEAR -1

//Max number of tilesets per map
#define TILESET_COUNT 4


//Default Tileset sizes
#define SPRITE_W 16
#define SPRITE_H 16
#define SPRITE_W_SMALL 8
#define SPRITE_H_SMALL 8
#define SPRITE_COUNT (TILESET_W/SPRITE_W * TILESET_H/SPRITE_W) 
#define SPRITE_SMALL_COUNT (TILESET_W/SPRITE_W_SMALL * TILESET_H/SPRITE_H_SMALL) 

#define DEFAULT_ANIMATION "default"
#define FPS_STAT_RATE  5.f 

#define BLACK Palette[0]
#define WHITE Palette[1]
#define CLEAR Color()
#define HIGHLIGHT Color( 255 / 2, 250, 250, 250 )
#define CURSOR_COLOR Palette[25]
#define BORDER_COLOR WHITE
//editor "background" color. This color should not be selectable
#define EDITOR_COLOR Palette[17]

#define CURSOR_FLICKER_RATE  3
