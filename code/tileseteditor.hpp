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

	void setTileset(const std::string & name);
	
	//commit change
	void redo();
	void undo();
	void save();

private:
	void commit();
	//draw the propoed changes with mouse at tilex,y
	void drawOverlay(int tilex, int tiley, const Rect & dest);
	

	std::string m_sheetName;
	Graphics::Tileset * m_sheet;
	const int m_tileScale = 8;
	UI::TilePicker * m_sheetPicker;
	UI::ColorPicker * m_colorPicker;
	UI::Toolbar * m_toolbar;

	//tileId button replace with text input
	UI::TextButton * m_tileIdBox;
	ToolMode m_tool;
	//shape bounds
	Rect m_shapeRect;
	//used to render the potential modifications 
	Graphics::Tileset * m_overlay;

	std::vector<Color * > m_revisionData;
	//selection id to revision count 
	//TODO
	//std::unordered_map<int, int > m_revisions; 
	int m_revision; 

	bool m_usingTool;

	int m_charW, m_charH;
};