#pragma once

//TODO This will be dll api

//Game Runtime API 
namespace Idolon
{
	//boot from cartridge. this will load data into ram. otherwise will use assets syste
	void Boot(const char * cartname);
	void Step();
	void Quit();

	////TODO - asset api to create/load assets from disk or datas. use 
	//void Load( const char *assetName )
	//{}
	//void Create( const char *assetName )
	//{}
	//
	//void Destroy( int id )
	//{}
	////if empty ,will use generated
	//void Save( int id, const char *assetName = "" )
	//{}

	//======== System API ===========
	//load from disc or cartirdge, returns asset id 
	int Load( const char *assetName );
	//unload. free
	void Unload( int assetId );

	void MousePosition( int & x, int & y );

	//execute  gamescript code
	bool Import( const char *filepath );

	// ==== Primitive Drawing api === 
	//get display size
	void GfxSize( int & w, int & h );
	// clear the display
	void GfxClear( unsigned char r, unsigned char g, unsigned char b  );
	//TODO - set pixel
	//void gfx_pixel( int x, int y, int r, int g, int b  );

	//======= map api  =========
	//load map into layer. unloads previous
	void SetLayer(int layer, int mapId);
	//set view into layer
	void SetView(int layer, int x, int y, int w, int h);
	//scroll map to x,y
	void ScrollTo(int layer, int x, int y);
	void ScrollBy(int layer, int dx, int dy);
	//set/get tile identifier at pixel xy
	int GetTile( int layer, int x, int y );
	int SetTile( int layer, int x, int y, int tile );

	
	//========= sprite api =========
	int Spawn(int tileId, int x, int y, bool isSmall = false);
	void Despawn( int spriteId );
	void MoveTo(int spriteId, int x, int y);
	void MoveBy(int spriteId, int dx, int dy);
	void FlipTo(int spriteId, int tileId);
	void FlipBy(int spriteId, int di);
	int GetFrame( int spriteId );
	bool GetSpritePosition( int spriteId, int &x, int &y );
	bool GetSpriteSize( int spriteId, int &w, int &h );
	
	//set sprite sheet
	void SetSpriteSheet( int tilesetId );

}