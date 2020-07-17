#pragma once

#include "ui.hpp"
#include "commands.hpp"

class Shell : public UI::App 
{
public:
	Shell();
	~Shell();
	void onEnter() override ;
	void onExit() override ;
	//process key event
	void onKey(Key key, bool isDown)override ;
	void onTick() override ;

	void clear();
	void log(const std::string & msg);
	void setFont(const std::string & font);
	Graphics::Font* getFont();
	void addCommands(const CommandTable & commands);

private:
	void showOption();

	void overrideInput(const std::string& msg);

	std::string m_fontName = DEFAULT_FONT;
	Graphics::Font* m_font;
	Graphics::TextBox * m_cursor, * m_input, * m_buffer;

	int m_charW, m_charH;
	int m_w, m_h;

	const int cursorFlickRate = 3;
	//when tab is pressed, ls fills this if empty, else goes to next option
	//if shift, goes to prev on enter this is cleared
	int m_option;
	std::vector<std::string> 	m_options;
	std::list<std::string> 	m_lines;
	int m_optionDirection;

	CommandTable m_commands;
	//current command
	std::string m_command;
	int m_lineW, m_lineH; //max number of chars

	int m_cursorPos;
	std::string m_stashedInput;
	float m_timer;
};