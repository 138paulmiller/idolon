#pragma once
#include "sys.hpp"


enum GameState : uint8  
{
	GAME_OFF,
	GAME_PAUSED,
	GAME_RUNNING,
};

//This should be the scripting Python API

/*
	game directory structure.

	game file is cartridge description file 

	path/to/demo.desc 
			-demo.tls
			-demo.map

	
	demo.game
		1 tilesets
		demo
		1 maps 
		demo
		128 codebytes
		import idolon as I
		...
		 
	build demo
		spits out cartridge which packs the asset data as well
	run demo

*/
namespace  Game
{
	//Create a loader for each asset type. given a block of data load the asset into the CPU
	//
	//these describe the game files.
	struct Desc : Asset 
	{
		//array of tileset names
		std::vector<std::string> tilesets;
		std::vector<std::string> maps;
		std::vector<std::string> scripts;
	};

	//create a mapping from tileset name to cartridge 

	struct Cartridge : Asset 
	{ 
		struct Header
		{
			std::unordered_map<std::string, uint> offsets;
		} 
		header;

		Cartridge(const std::string & name, const Desc * desc, char * m_data );
		~Cartridge();	

		Graphics::Map * LoadMap(const std::string & mapname);

	private:
		char * m_data;
		const Desc * m_desc;
	};


	void Startup(const std::string & cartpath);

	//call script init
	void Init();
	
	//call update and 
	GameState Update();
	
	void Shutdown();

	//load map into layer. unloads previous
	void Load(Layer layer, const std::string & mapname);
	void Unload(Layer layer);
	void Resize(Layer layer, int w, int h);
	void Scroll(Layer layer, int x, int y);
	
	//Use integer to pass to scrits. large uses large sprite 
	int Spawn(int tileId, int x, int y, bool isSmall = false);
	//despawn sprite
	void Despawn(int spriteId);
	Graphics::Sprite * GetSprite(int spriteId);
	void UseSpriteSheet(const std::string & tileset);
	
}