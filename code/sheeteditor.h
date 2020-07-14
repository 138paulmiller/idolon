#pragma once
#include "ui.h"


enum ToolMode : char
{
	//the correspond to tool bar order!. can be used by APP to get widget
	TOOL_PIXEL = 0,
	TOOL_FILL,
	TOOL_LINE,
	TOOL_COUNT,
};

//Edit sheet should be renamed TileSheetEditor
class  SheetEditor : public UI::App 
{
public:
	SheetEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;

	void setSheet(const std::string & name);
	
	//commit change
	void redo();
	void undo();
	void save();

private:
	void commit();
	//draw the propoed changes with mouse at tilex,y
	void drawOverlay(int tilex, int tiley, const Rect & dest);
	

	std::string m_sheetName;
	Graphics::Sheet * m_sheet;
	const int m_tileScale = 8;
	UI::SheetPicker * m_sheetPicker;
	UI::ColorPicker * m_colorPicker;
	UI::Toolbar * m_toolbar;
	ToolMode m_tool;
	//shape bounds
	Rect m_shapeRect;
	//used to render the potential modifications 
	Graphics::Sheet * m_overlay;

	std::vector<Color * > m_revisionData;
	//selection id to revision count 
	//TODO
	//std::unordered_map<int, int > m_revisions; 
	int m_revision; 

	bool m_usingTool;
};