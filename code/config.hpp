#pragma once
//TODO create settings manager in engine

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
#define FPS_STAT_RATE  5.f 
#define GAME_API_UPDATE "update"
#define GAME_API_INIT "init"

#define DEFAULT_COLOR_TEXT  WHITE
#define DEFAULT_COLOR_FILL  BLACK
#define DEFAULT_COLOR_CLICK Palette[26] 
#define DEFAULT_COLOR_HOVER Palette[25]
#define DEFAULT_TEXT_BORDER 2

#define DEFAULT_FONT_EXT ".fnt"
#define DEFAULT_MAP_EXT ".map" 
#define DEFAULT_TILESET_EXT ".tls"
#define DEFAULT_SCRIPT_EXT ".scr"
#define DEFAULT_GAMEDESC_EXT  ".cfg"
#define DEFAULT_RAW_EXT  ".raw"

#define CACHE_PATH "__cache__"
