#pragma once

#include "context.h"

/*
	Primary Editor interface. 
*/

class Editor : public UI::App
{
public:
	Editor(uint8_t support = 0);
	virtual void redo();
	virtual void undo();
	virtual void save();
	
	virtual void onEnter();
	virtual void onExit();

	void reloadMenu();
	bool supports(AppSupport support);
private:
	uint8_t m_support;
	int m_menu;

};




