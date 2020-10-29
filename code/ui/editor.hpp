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

	//returns string representation of icon
	static std::string getIcon( const std::string &name );

	virtual void onRedo();
	virtual void onUndo();
	virtual void onSave();
	
	virtual void onEnter() override;
	virtual void onExit() override;

	bool supports(AppSupport support);
	void hideControl( bool hidden );
	void hideTools(bool hidden);
	void addTool(const std::string & text, std::function<void()> click, bool sticky=true );
	inline int controlX() { return m_controlX;  }
	inline int controlY() { return m_controlY;  }


private:

	uint8 m_support;
	int m_control;
	int m_toolbar;
	int m_controlX;
	int m_controlY;
	bool m_resetToolbar;

};




