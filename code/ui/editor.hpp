#pragma once

#include "context.hpp"

/*
	Primary Editor interface. 
*/

class Editor : public App
{
public:
	Editor(uint8 support = 0);
	virtual void redo();
	virtual void undo();
	virtual void save();
	
	virtual void onEnter();
	virtual void onExit();

	bool supports(AppSupport support);

	void hideControl( bool hidden );
	void hideTools(bool hidden);
	void addTool(const std::string & text, std::function<void()> click, bool sticky=true );
	inline int controlX() { return m_controlX;  }
	inline int controlY() { return m_controlY;  }

private:

	uint8 m_support;
	int m_control;
	int m_controlX;
	int m_controlY;
	int m_toolbar;
	bool m_resetToolbar;

};




