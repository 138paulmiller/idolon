#pragma once
#include "sys.hpp"


//This should be the primary Python API
namespace  Game
{
	//Create a loader for each asset type. given a block of data load the asset into the CPU
	//
	struct Cartridge
	{ 
		//these describe the game files.
		struct Desc
		{
			//array of tileset names
			int tilesetCount;
			char ** tilesets;
			int mapCount;
			char ** maps;
			char * scriptCount;
			char ** scripts;
		} desc;
	Cartridge(const std::string & cartfile );	
	private:
		char * m_data;
	};
	/*
	Memory layout
	Tileset (4096 bytes)
	
	0x00000 - 0x03FFF : Tileset 0	
	0x04000 - 0x07FFF : Tileset 1	
	0x08000 - 0x0BFFF : Tileset 2	
	0x0C000 - 0x0FFFF : Tileset 3
	
	*/

	//create a swap function that loads from cartridge into CPU.
	// the tileset index is specified in the map section of the cartridge, so when loading a map, also load the tileset index
	class CPU
	{
		char * tilesets[LAYER_COUNT];
	};
	
	struct Page //similar to extent window over data
	{
		int offset;
		int size;
	};

	//all asset files in the desc are collated into this single block of memory
	//encode within png ?
	
	union MMU 
	{
		Page * pages;
		int pageCount;
		//todo use mmap
		char * data;
	} mmu;

	//maps from tileset/map name to page. 
	std::unordered_map<char *, Page> m_memmap; 



	void Startup(const std::string & cartpath);
	void Shutdown();

	//load map into layer. unloads previous
	void Load(Layer layer, const std::string & mapname);
	void Resize(Layer layer, int x, int y);
	void Scroll(Layer layer, int x, int y);
	
	//Sprite Manager 
	//spawn sprite at x y
	Graphics::Sprite * Spawn(int tileid, int x, int y);
	//despawn sprite
	void Despawn(const Graphics::Sprite * sprite);


	
}