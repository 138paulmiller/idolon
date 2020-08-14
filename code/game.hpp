#pragma once
#include "sys.hpp"


enum GameState : uint8  
{
	GAME_OFF,
	GAME_PAUSED,
	GAME_RUNNING,
};

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

	void Startup(const std::string & gameName);
	GameState Run();
	void Shutdown();

	//load map into layer. unloads previous
	void Load(Layer layer, const std::string & mapname);
	void Unload(Layer layer);
	void Resize(Layer layer, int x, int y);
	void Scroll(Layer layer, int x, int y);
	
	//Use integer to pass to scrits. large uses large sprite 
	int Spawn(int tileid, int x, int y, bool isSmall = false);
	//despawn sprite
	void Despawn(int spriteId);
	Graphics::Sprite * GetSprite(int spriteId);
	void UseSpriteSheet(const std::string & tileset);


	
}