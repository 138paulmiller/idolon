
#define ERR(...) \
	LOG(__VA_ARGS__)

#define BINDINGS_CONSTS  R"(
BG=0
SP=1
FG=2
UI=3
RELEASE = 0
CLICK = 1
HOLD = 2
ESCAPE = '\033'
SHIFT = 127
ALT = SHIFT + 1
UP = ALT + 1
DOWN = UP + 1
LEFT = DOWN + 1
RIGHT = LEFT + 1
)"


#define BINDINGS \
	DECL( idolon, log     , "Log message to debug console and file"    	)\
	DECL( idolon, scrw    , "Get screen width "                           )\
	DECL( idolon, scrh    , "Get screen height "                          )\
	DECL( idolon, mx      , "Get mouse x position"                        )\
	DECL( idolon, my      , "Get mouse y position"                        )\
	DECL( idolon, clear   , "Clear screen with color r,g,b"               )\
	DECL( idolon, key     , "Get key state. 0 is up, 1 is down. 2 is hold")\
	DECL( idolon, load    , "Load layer"                                  )\
	DECL( idolon, unload  , "Unload layer"                                )\
	DECL( idolon, view    , "Set the layer viewport"                      )\
	DECL( idolon, scroll  , "Scroll layer by dx,dy"                       )\
	DECL( idolon, sprite  , "Spawn sprite "                               )\
	DECL( idolon, kill    , "Despawn sprite "                             )\
	DECL( idolon, pos     , "Get or set sprite position"                  )\
	DECL( idolon, move    , "Move sprite by x,y"                          )\
	DECL( idolon, frame   , "Get or set sprite current tile"              )\
	DECL( idolon, flip    , "Flip sprite tile by di"                      )\
	DECL( idolon, sheet   , "Set surrent sprite sheetsprite "             )\


