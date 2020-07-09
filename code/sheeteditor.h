#pragma once
#include "ui.h"

//Edit sheet should be renamed TileSheetEditor
class  SheetEditor : public UI::App 
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
	const int m_tileScale = 8;
	UI::SheetPicker * m_sheetPicker;
	UI::ColorPicker * m_colorPicker;
};