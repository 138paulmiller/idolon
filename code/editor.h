#pragma once

#include "context.h"


class Toolbar : public UI::App
{
public:
	Toolbar();
private:

};

class Editor : public UI::App
{
public:
	Editor();

	void onEnter();
	void onExit() ;
	void onTick() ;
	void onKey(Key key, bool isDown) ;
	void editSheet(const std::string & sheetname);

	enum : uint8_t
	{
		VIEW_SHEET_EDITOR = 0,
		VIEW_COUNT,
	};

	void switchView(int viewId);
private:

	std::string m_sysPath;
	std::string m_sysAssetPath;
	//view context
	Context m_context;
	class SheetEditor * m_sheetEditor;

};




