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

	void reloadMenu();
	bool supports(AppSupport support);
private:
	uint8 m_support;
	int m_menu;

};




