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
	
	//execute  gamescript code
	bool Use( const char *filepath );
	void DisplaySize( int & w,int & h );
	void MousePosition( int & x,int & y );
		
	// ==== Primitive Drawing api === 
	// clear the display
	void Clear( unsigned char r, unsigned char g, unsigned char b  );

	//TODO - set pixel
	//void SetPixel( int x, int y, int r, int g, int b  );

	//======= map api  =========
	//load map into layer. unloads previous
	void Load(int layer, const char * mapname);
	void Unload(int layer);
	//set view into layer
	void View(int layer, int x, int y, int w, int h);
	//scroll map to x,y
	void Scroll(int layer, int x, int y);
	//get tile identifier at pixel xy
	int GetTile( int layer, int x, int y );
	//get tile identifier at pixel xy
	void SetTile( int layer, int x, int y, int tile );
	
	//========= sprite api =========
	int Spawn(int tileId, int x, int y, bool isSmall = false);
	void Despawn( int spriteId );
	void MoveTo(int spriteId, int x, int y);
	void MoveBy(int spriteId, int dx, int dy);
	void FlipTo(int spriteId, int tileId);
	void FlipBy(int spriteId, int di);
	int Frame( int spriteId );
	bool Position( int spriteId, int &x, int &y );
	bool SpriteSize( int spriteId, int &w, int &h );
	
	//set sprite sheet
	void Sheet(const char *  tileset);


}