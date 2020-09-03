#pragma once
#include "engine/api.hpp"

namespace Runtime
{
	//load map into layer. unloads previous
	void Load(int layer, const char * mapname);
	void Unload(int layer);
	void Resize(int layer, int w, int h);
	void Scroll(int layer, int x, int y);
	
	//draw sprite
	void Sprite(int tileId, int x, int y, bool isSmall = false);
	
	//set sprite sheet
	void Sheet(const char *  & tileset);
}