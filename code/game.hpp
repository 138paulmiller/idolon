#pragma once
#include "engine/api.hpp"


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
	struct Header : public Asset
	{
		Header( const std::string &name );
		//game name
		
		//array of tileset names
		std::vector<std::string> fonts;
		std::vector<std::string> tilesets;
		std::vector<std::string> maps;
		std::vector<std::string> scripts;
	} ;


	struct Chunk
	{
		~Chunk() { delete data; }
		//asset offset in chunk
		std::unordered_map<std::string, int> offsets;
		char *data = 0;
		int size = 0 ;
	};

	//create a mapping from tileset name to cartridge 
	//create cart factory
	class Cartridge
	{  
	public:

		Cartridge( const Header &header, const Chunk &chunk );

		~Cartridge();	

		Asset* LoadImpl( const std::type_info& type, const std::string& name);
		void UnloadImpl( const std::type_info& type, Asset*  asset);


		template <typename AssetType>
		AssetType *Load( const std::string &name )
		{
			return dynamic_cast<AssetType*>(LoadImpl( typeid(AssetType), name));
		}
		
		template <typename AssetType>
		void Unload( Asset* asset )
		{			
			UnloadImpl( typeid(AssetType), asset);
		}

		Header header;
		Chunk chunk;

	private:
	};

	void Startup(const std::string & cartpath);
	void Shutdown();

	//package game into cartridge
	void Package( const Game::Header & header, const std::string & cartpath);
	Cartridge * Unpackage( const std::string &cartpath );
	
}