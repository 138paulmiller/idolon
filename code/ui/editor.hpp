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
	
	void addTool(const std::string & text, std::function<void()> click, bool sticky );
	inline int menuY() { return m_menuY;  }

private:

	uint8 m_support;
	int m_menuY;
	int m_menu;
	int m_toolbar;

};




