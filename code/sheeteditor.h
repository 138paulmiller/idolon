#pragma once
#include "ui.h"

//Edit sheet should be renamed TileSheetEditor
class  SheetEditor : public UI 
{
public:

	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;

	void setSheet(const std::string & name);
private:
	
	std::string m_sheetName;
	Graphics::Sheet * m_sheet;
	int m_tileScale = 5;
	SheetPicker * m_sheetPicker;
	int m_colorIndex = 4;
};