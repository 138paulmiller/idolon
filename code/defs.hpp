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

#define MAP_W 16
#define MAP_H 16


//Default Tileset sizes
#define SPRITE_W 16
#define SPRITE_H 16
#define SPRITE_W_SMALL 8
#define SPRITE_H_SMALL 8
#define SPRITE_COUNT (TILESET_W/SPRITE_W * TILESET_H/SPRITE_W) 
#define SPRITE_SMALL_COUNT (TILESET_W/SPRITE_W_SMALL * TILESET_H/SPRITE_H_SMALL) 

#define DEFAULT_ANIMATION "default"