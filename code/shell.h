#pragma once

#include "ui.h"
#include "commands.h"

class Shell : public UI 
{
public:
	
	Shell();
	~Shell();
	void onEnter() override ;
	void onExit() override ;
	//process key event
	void onKey(Key key, bool isDown)override ;
	void onTick() override ;


	void overrideInput(const std::string& msg);
	void log(const std::string & msg);
	void setFont(const std::string & font);
	void addCommands(const CommandTable & commands);
private:
	std::string m_fontName = "default";
	Graphics::Font* m_font;
	Graphics::TextBox * m_cursor, * m_input, * m_buffer;

	int m_charW, m_charH;
	int m_w, m_h;

	const int cursorFlickRate = 3;

	std::list<std::string> 	m_lines;

	CommandTable m_commands;
	//current command
	std::string m_command;
	int m_lineW;
	int m_cursorPos;
	std::string m_stashedInput;
	int m_timer;

};