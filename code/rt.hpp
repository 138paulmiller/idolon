#pragma once

//Game Runtime API 
namespace Runtime
{
	//boot from cartridge. this will load data into ram. otherwise will use assets syste
	void Boot(const char * cartname);
	void Step();
	void Quit();


	//======= map api  =========
	//load map into layer. unloads previous
	void Load(int layer, const char * mapname);
	void Unload(int layer);
	//set view into layer
	void View(int layer, int x, int y, int w, int h);
	//scroll map to x,y
	void Scroll(int layer, int x, int y);
	//get tile identifier at pixel xy
	int TileAt( int layer, int x, int y );
	
	//========= sprite api =========
	int Spawn(int tileId, int x, int y, bool isSmall = false);
	void Despawn( int spriteId );
	void MoveTo(int spriteId, int x, int y);
	void MoveBy(int spriteId, int dx, int dy);
	void FlipTo(int spriteId, int tileId);
	void FlipBy(int spriteId, int di);
	int Frame( int spriteId );
	bool Position( int spriteId, int &x, int &y );
	//set sprite sheet
	void Sheet(const char *  tileset);


}