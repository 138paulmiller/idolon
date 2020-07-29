#pragma once
#include "sys.hpp"
#include "editor.hpp"


enum ToolMode : char
{
	//the correspond to tool bar order!. can be used by APP to get widget
	TOOL_PIXEL = 0,
	TOOL_FILL,
	TOOL_LINE,
	TOOL_ERASE,
	TOOL_COUNT,
};

class  TilesetEditor : public Editor 
{
public:
	TilesetEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;

	void redo()override;
	void undo()override;
	void save()override;

	//allow for runtime update of tilesheet
	void setTileset(const std::string & name);
private:
	void commit();
	//draw the propoed changes with mouse at tilex,y
	void drawOverlay(int tilex, int tiley, const Rect & dest);
	

	UI::TilePicker * m_tilepicker;
	UI::ColorPicker * m_colorpicker;
	UI::Toolbar * m_toolbar;
	UI::TextButton * m_tileIdBox;
	
	std::string m_tilesetName;
	Graphics::Tileset * m_tileset;
	const int m_tileScale = 8;
	
	bool m_usingTool;
	ToolMode m_tool;
	//shape is data used by the current tool. for line xy = x1y1 wh = x2y2
	Rect m_shapeRect;
	//used to render the potential modifications 
	Graphics::Tileset * m_overlay;

	int m_revision; 
	std::vector<Color * > m_revisionData;
	//
	int m_charW, m_charH;
};