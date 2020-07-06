#pragma once
#include "ui.h"

//Create terminal system. Echos key input to a buffer. has line offset nd render the text to the screen

class  EditSheet : public UI 
{
public:

	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;

	void setSheet(const std::string & name);
private:
	std::string s_sheetname;
	Graphics::TextBox * s_text, * s_text2;
	int mx, my;
	int w, h;
};