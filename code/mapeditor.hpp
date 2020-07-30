#pragma once
#include "sys.hpp"
#include "editor.hpp"

//MapTools
enum MapToolMode : char
{
	//the correspond to tool bar order!. can be used by APP to get widget
	MAP_TOOL_PIXEL = 0,
	MAP_TOOL_FILL,
	MAP_TOOL_LINE,
	MAP_TOOL_ERASE,
	MAP_TOOL_COUNT,
};

class  MapEditor : public Editor 
{
public:
	MapEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;

	void redo() override;
	void undo() override;
	void save() override;
	
	//
	void setMap( const std::string& name );

	
private:
	//"highlights cursor"
	//x y in pixel space
	void drawCursor(int x, int y);

	//by default are all empty
	std::string m_mapName;
	//map contains ref to tilesheet
	Graphics::Map * m_map;
	UI::TilePicker * m_tilepicker;
	//DEBUG
	std::vector<Graphics::Sprite*> m_sprites;

	//previous mouse xy
	int m_prevmx,m_prevmy;

	bool m_shift = false;
	//load 4 tiles ! 

};