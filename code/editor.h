#pragma once

#include "ui.h"


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

	template <typename Type>
	Type * getView(int viewId)
	{
		return dynamic_cast<Type*>(m_views[viewId]);
	}

	void switchView(int viewId);
private:

	std::string m_sysPath;
	std::string m_sysAssetPath;
	//current working dir
	int m_prevViewId;
	int m_viewId;
	UI::App * m_view;
	UI::App * m_views[VIEW_COUNT];
};




