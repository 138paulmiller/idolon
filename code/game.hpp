#pragma once
#include "sys.hpp"

//Each layer is a tileset in memory
enum Layer
{
	//In Draw Order
	LAYER_BG=0, //background layer
	LAYER_SP, //sprite layer
	LAYER_FG, //foregroud layer
	LAYER_UI, //user-interface layer
};

//This should be the primary Python API
namespace  Game
{
	class Cartridge
	{
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