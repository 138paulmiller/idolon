#pragma once

//TODO This will be dll api

//Game Runtime API 
namespace Idolon
{
	enum { DRAWMODE_PIXEL = 0, DRAWMODE_TILED };

	//boot from cartridge. this will load data into ram. otherwise will use assets syste
	void Boot(const char * cartname);
	void Draw();
	void Quit();

	//======== System API ===========
	//load from disc or cartirdge, returns asset id 
	int Load( const char *assetName );
	//unload. free
	void Unload( int assetId );

	void MousePosition( int & x, int & y );
	
	int GetKeyState( char keysym );
		
	//execute  gamescript code
	bool Import( const char *filepath );
	//TODO - create a serialization api to write into a user file.


	// ==== Primitive Drawing api === 
	//get display size
	void GfxSize( int & w, int & h );
	//set draw mode
	void GfxDrawMode( int mode );
	// clear the display
	void GfxClear( unsigned char r, unsigned char g, unsigned char b  );
	int GfxCreateTexture( int width, int height );
	void GfxDestroyTexture( int textureId );
	//a,r,g,b formatted array
	void GfxUpdateTexture( int textureId, const unsigned char *pixels, int width, int x, int y, int w, int h );
	
	void GfxDrawTexture( int textureId,
		int sx, int sy, int sw, int sh,
		int dx, int dy, int dw, int dh
	);
	
	void GfxDrawLine(
		unsigned char r, unsigned char g, unsigned char b, unsigned char a,
		int x1, int y1, int x2, int y2
	);
	
	void GfxDrawRect(
		unsigned char r, unsigned char g, unsigned char b, unsigned char a,
		int x, int y, int w, int h, bool filled
	);

	//======= map api  =========
	//load map into layer. unloads previous. to unload just set mapId to -1
	void MapLoad(int layer, int mapId);
	//set view into layer
	void MapView(int layer, int x, int y, int w, int h);
	//scroll map to x,y
	void MapScrollTo(int layer, int x, int y);
	void MapScrollBy(int layer, int dx, int dy);
	//set/get tile identifier at pixel xy
	int MapGetTile( int layer, int x, int y );
	int MapSetTile( int layer, int x, int y, int tile );

	
	//========= sprite api =========
	int SprSpawn(int tileId, int x, int y, bool isSmall = false);
	void SprDespawn( int spriteId );
	void SprSetPosition(int spriteId, int x, int y);
	void SprSetFrame(int spriteId, int tileId);
	int SprGetFrame( int spriteId );
	bool SprGetPosition( int spriteId, int &x, int &y );
	bool SprGetSize( int spriteId, int &w, int &h );
	
	//set sprite sheet
	void SprSetSheet( int tilesetId );

}