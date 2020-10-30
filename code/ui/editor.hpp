#pragma once

#include "app.hpp"

/*
	Primary Editor interface. 
	Replace toolbar with tooltip menu! This will display on right click and disaapear on click. 

*/

class Editor : public App
{
public:

	Editor(uint8 support = 0);
	
	virtual void onEnter() override;
	virtual void onExit() override;

	bool supports(AppSupport support);
	void hideControl( bool hidden );
	void hideTools(bool hidden);
	void addTool(const std::string & text, std::function<void()> click, bool sticky=true );

private:

	uint8 m_support;
	int m_control;
	int m_toolbar;
	int m_controlX;
	int m_controlY;
	bool m_resetToolbar;

};




