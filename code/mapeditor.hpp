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
	
	//line xy
	int x1, y1, x2, y2;
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
	void setTileset(int index, const std::string& tileset );

	void useTileset(int index );

	//toolbar and tilepicker. resize map
	void hideWorkspace() ;

	void drawOverlay() ;

private:
	//loaded tilesets. map can only use one tileset at a time

	//"highlights cursor"
	//x y in pixel space. aligned to tiles
	bool handleScroll();
	void handleTool();
	//by default are all empty
	std::string m_mapName;
	
	UI::TextInput * m_tilesetInput;
	//map contains ref to tilesheet
	Graphics::Map * m_map;
	UI::TilePicker * m_tilepicker;
	UI::Toolbar * m_toolbar;
	//
	UI::Toolbar * m_tilesetSelectToolbar;
	int m_tilesetSelection;

	//DEBUG
	std::vector<Graphics::Sprite*> m_sprites;

	//previous mouse xy
	int m_prevmx,m_prevmy;

	bool m_shift = false;
	//load 4 tiles ! 
	MapToolMode m_tool;
	//rect used by tool
	ToolData m_tooldata;
	//used to render the potential modifications 
	//use a map ? copy indices over for floodfill
	Graphics::Tileset * m_overlay;

};