#pragma once
/*
*/

#include "ui/api.hpp"

//
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
std::unordered_map<char *, Page> s_memmap;

//This is an abstraction to run the game using local assets rather than 
namespace Emu
{
	void Startup();
	void Shutdown();


	//Instruction interface 
	//void Load Cartridge file 
}