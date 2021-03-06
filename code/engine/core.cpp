
#include "core.hpp"


std::string Trim(const std::string &s)
{
	int i = 0,j = s.size(); 
	while(i < s.size() && isspace(s[i]) ) i++;
	while(j > 0 && isspace(s[j-1]) ) j--;
	if(i == s.size()) i = s.size()-1;
	if(j == 0) j=0;
	return s.substr(i,j);
}

bool Rect::intersects(const Rect & other) const
{
	return x < other.x+other.w && x+w > other.x 
		&& y < other.y+other.h && y+h > other.y;
}

Color::Color( uint8 a, uint8 r, uint8 g, uint8 b )
 : a(a), r(r), g(g),b(b)
{

}

bool Color::operator!=(const Color & c) const
{
	return !(c == *this);
} 
bool Color::operator==(const Color & c) const
{
	return c.a == a 
		&& c.r == r 
		&& c.g == g 
		&& c.b == b;
} 

const Color Palette[32] =
{
	{0xff, 0x00, 0x00, 0x00},
	{0xff, 0xff, 0xff, 0xff},
	{0xff, 0x12, 0x25, 0x24},
	{0xff, 0x22, 0x20, 0x34},
	{0xff, 0x45, 0x28, 0x3c},
	{0xff, 0x66, 0x39, 0x31},
	{0xff, 0x8f, 0x56, 0x3b},
	{0xff, 0xdf, 0x71, 0x26},
	{0xff, 0xd9, 0xa0, 0x66},
	{0xff, 0xee, 0xc3, 0x9a},
	{0xff, 0xfb, 0xf2, 0x36},
	{0xff, 0x99, 0xe5, 0x50},
	{0xff, 0x6a, 0xbe, 0x30},
	{0xff, 0x37, 0x94, 0x6e},
	{0xff, 0x4b, 0x69, 0x2f},
	{0xff, 0x52, 0x4b, 0x24},
	{0xff, 0x32, 0x3c, 0x39},
	{0xff, 0x3f, 0x3f, 0x74},
	{0xff, 0x30, 0x60, 0x82},
	{0xff, 0x5b, 0x6e, 0xe1},
	{0xff, 0x63, 0x9b, 0xff},
	{0xff, 0x5f, 0xcd, 0xe4},
	{0xff, 0xcb, 0xdb, 0xfc},
	{0xff, 0x9b, 0xad, 0xb7},
	{0xff, 0x84, 0x7e, 0x87},
	{0xff, 0x69, 0x6a, 0x6a},
	{0xff, 0x59, 0x56, 0x52},
	{0xff, 0x76, 0x42, 0x8a},
	{0xff, 0xac, 0x32, 0x32},
	{0xff, 0xd9, 0x57, 0x63},
	{0xff, 0xd7, 0x7b, 0xba},
	{0xff, 0x8a, 0x6f, 0x30},
};
const int PaletteCount = 32;


const std::string &TranslateIcon( const std::string &name )
{
	//TODO move this table to ini file!!
	//map from icon name to tileindex repr
	static std::unordered_map<std::string, std::string> s_iconNameMap = {
		{ "PENCIL", { 0  } },
		{ "FILL",   { 1  } },
		{ "LINE",   { 2  } },
		{ "ERASER", { 3  } },
		{ "UNDO",   { 4  } },
		{ "REDO",   { 5  } },
		{ "RELOAD", { 6  } },
		{ "SAVE",   { 7  } },
		{ "LOAD",   { 8  } },
		{ "EXIT",   { 9  } },
		{ "STAMP",  { 10 } },
		{ "BUILD",  { 11 } },
		{ "PLAY",   { 12 } },
		{ "PAUSE",  { 13 } },
		{ "DOWN",   { 14 } },
		{ "UP",     { 15 } },
		{ "TERMINAL",   { 16 } },
		{ "CODE",   { 17 } },
		{ "TILESET",   { 18 } },
		{ "MAP",   { 19 } },
		{ "ADD",   { 20 } },
		{ "REMOVE",   { 21 } },
		{ "PACKAGE",   { 22 } },
		{ "CONFIG",   { 23 } },
	};
	auto iconIt = s_iconNameMap.find( name );
	if ( iconIt != s_iconNameMap.end() )
	{
		return iconIt->second;
	}
	//assert!
	return "";
}