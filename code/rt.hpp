#pragma once

//Game Runtime API 
namespace Runtime
{
	//boot from cartridge. this will load data into ram. otherwise will use assets syste
	void Boot(const char * cartname);
	void Step();
	void Quit();

	//load map into layer. unloads previous
	void Load(int layer, const char * mapname);
	void Unload(int layer);
	void Resize(int layer, int w, int h);
	void Scroll(int layer, int x, int y);
	
	//sprite api
	int Spawn(int tileId, int x, int y, bool isSmall = false);
	void Despawn( int spriteId );

	void MoveTo(int spriteId, int x, int y);
	void MoveBy(int spriteId, int dx, int dy);
	void FlipTo(int spriteId, int tileId);
	void FlipBy(int spriteId, int di);
	void Sheet(const char *  tileset);
	int Frame( int spriteId );
	bool Position( int spriteId, int &x, int &y );

	int TileAt( int layer, int x, int y );

}