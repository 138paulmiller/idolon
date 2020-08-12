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
		Cartridge(const Desc & desc );
		~Cartridge();	
	private:
		char * m_data;
	};

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