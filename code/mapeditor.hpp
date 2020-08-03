#pragma once
#include "sys.hpp"
#include "editor.hpp"
/*
	TODO - Show hide tile editor and toolbar
	add collision shapes with ids. 

*/
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

struct ToolData
{
	//mouse xy
	int mx, my;
	//size 
	int w, h;  
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
	//
	void setTileset(const std::string& tileset );

	//toolbar and tilepicker
	void hideWorkspace() ;

private:
	//loaded tilesets. map can only use one tileset at a time

	//"highlights cursor"
	//x y in pixel space. aligned to tiles
	bool handleScroll();
	void handleTool();
	//by default are all empty
	std::string m_mapName;
	
	//map contains ref to tilesheet
	Graphics::Map * m_map;
	UI::TilePicker * m_tilepicker;
	UI::Toolbar * m_toolbar;

	//DEBUG
	std::vector<Graphics::Sprite*> m_sprites;

	//previous mouse xy
	int m_prevmx,m_prevmy;

	bool m_shift = false;
	//load 4 tiles ! 
	MapToolMode m_tool;
	//rect used by tool
	ToolData m_tooldata;

};