#include "shell.h"
#include "engine.h"
#include "graphics.h"
#include "fs.h"
#include "assets.h"
#include <sstream>

#ifndef SHELL_PREFIX
#define SHELL_PREFIX ">"
#define SHELL_PREFIXSIZE sizeof(SHELL_PREFIX)-1
#endif

Shell::Shell() 
{

}


Shell::~Shell() 
{
}

void Shell::onEnter()
{
	printf("Entering shell...");

	Engine::GetSize(m_w, m_h);

	m_font = Assets::Load<Graphics::Font>(m_fontName );
	m_charW = m_font->charW;
	m_charH = m_font->charH;

	m_cursor = new Graphics::TextBox(1, 1, " ");
	m_cursor->font = m_fontName ;
	m_cursor->filled = true;
	m_cursor->fillColor = WHITE ;
	m_cursor->reload();



	m_lineW = m_w / m_charW;
	//space for input line 
	m_lineH = m_h / m_charH - 1;

	m_input = new Graphics::TextBox(m_lineW, 1, SHELL_PREFIX);
	m_input->font = m_fontName ;    
	m_input->reload();

	//draw buffer as the entire background. last line is rendered but never filled
	m_buffer = new Graphics::TextBox(m_lineW, m_lineH+1, "");
	m_buffer->font = m_fontName ;
	m_buffer->x = 0;
	m_buffer->y = 0; 
	m_buffer->fillColor = BLACK ;       
	m_buffer->filled = true;
	m_buffer->reload();

	m_cursorPos = SHELL_PREFIXSIZE; //offset by > 
	m_stashedInput = "";
	m_timer = 0;
	m_optionDirection = 1;

	//Persist buffer input
/*	std::string linesText;
	for(const std::string & line : m_lines)
	{
		linesText += line + "\n";
	}
	if(linesText.size())
		log(linesText);
*/
	m_lines.clear();
	Engine::ClearScreen();
}

void Shell::onExit()
{
	clear();
	Assets::Unload(m_fontName );
	if(m_input)
	{
		delete m_input;
		m_input  = 0;	
	}  
	if(m_cursor)
	{
		delete m_cursor;
		m_cursor = 0;
	} 
	if(m_buffer) 
	{
		delete m_buffer;
		m_buffer = 0;
	}	
	printf("Exited Shell\n");

}

void Shell::onKey(Key key, bool isDown)
{

	if (!isDown)
	{
		switch(key)
		{
		case KEY_SHIFT:
			m_optionDirection = 1;
			break;
		default: break;
		}
	}
	else 
	{

		switch (key)
		{
		case KEY_SHIFT:
			m_optionDirection = -1;
			break;
		case KEY_LEFT:
			if(m_cursorPos > 1) 
				m_cursorPos--;					
			break;
		case KEY_RIGHT:
			if(m_cursorPos < m_input->text.size()) 
				m_cursorPos++;					
			break;
		case KEY_UP:
		case KEY_DOWN:
			break;
		case KEY_TAB:
			showOption();				
			break;
		case KEY_BACKSPACE:
			//do not remove >
			if (m_input->text.size() > SHELL_PREFIXSIZE)
			{
				if (m_cursorPos >= m_input->text.size())
					m_input->text.pop_back();
				else
					m_input->text.erase(m_cursorPos, 1);
				if(m_cursorPos > SHELL_PREFIXSIZE)
					m_cursorPos--;
			}
			m_options.clear(); //clear options
			break;
		case KEY_RETURN:
			log(m_input->text);
			if (m_input->text.size() > SHELL_PREFIXSIZE)
				m_command = m_input->text.substr(SHELL_PREFIXSIZE);
			m_cursorPos = SHELL_PREFIXSIZE; //reset
			m_input->text = SHELL_PREFIX;
			m_options.clear(); //clear options
			break;
		default:
			if (m_cursorPos < m_lineW-SHELL_PREFIXSIZE)
			{
				if (m_cursorPos == m_input->text.size()) 
					m_input->text += key;
				else
					m_input->text.insert(m_cursorPos, 1, key);
				m_cursorPos++;
			}
			m_options.clear(); //clear options

		}
		m_input->refresh();

	}
}

void Shell::onTick()
{
	m_timer += Engine::GetTimeDeltaMs()/1000.0f;
	if (m_timer > 1.0/cursorFlickRate) 
	{
		m_cursor->visible = !m_cursor->visible;
		m_timer = 0;
	}

	m_cursor->x = m_cursorPos * m_charW;	
	m_cursor->y = m_input->y;	

	m_buffer->draw();
	m_input->draw();
	m_cursor->draw();

	//process command at the end of the frame
	if(m_command.size())
	{
		Execute(m_command, m_commands);
		m_command = "";
	}
}

void Shell::addCommands(const CommandTable & commands)
{
	m_commands.insert(commands.begin(), commands.end());
}

void Shell::setFont(const std::string & font)
{
	m_fontName = font;	
	onExit();
	onEnter();
}
void Shell::overrideInput(const std::string& msg)
{
	//should wrap ? 
	m_input->text = msg; 
	m_input->refresh();
	m_cursorPos = m_input->text.size();
}
void Shell::clear()
{
	m_input->y = m_charH;
	m_buffer->text = "";
	m_lines.clear();
	log("");
}

//Can only be called in command
void Shell::log(const std::string & msg)
{
	//split into lines
	std::string line;
	std::istringstream iss;
	iss.str(msg);
	m_buffer->text = ""; //reset buffer. will be rebuilt
	while (std::getline(iss, line, '\n'))
	{
		//move input down, if cannot go down further pop lines off buffer
		m_lines.push_back(line);
		if (m_lines.size() >= m_lineH)
			m_lines.pop_front();
		else
			m_input->y += m_charH;
	}

	for (const std::string& line : m_lines)
		m_buffer->text += line + '\n';

	m_buffer->refresh();

			
}

void Shell::showOption()
{
	std::vector<std::string> options;
	FS::Ls(options);
	for(auto command : m_commands)
	{
		options.push_back(command.first.name);
	}
	//if files exist, and input line has at least >, and no options. then fill options
	if(options.size() && m_input->text.size() > 0 && m_options.size() == 0)
	{
		m_options.reserve(options.size());
		int inputLen = m_input->text.size();
		//move input to last "arg"
		int pos = inputLen-1;
		//remove >
		std::string arg = m_input->text.substr(SHELL_PREFIXSIZE);
		std::string command = "";

		pos = arg.find_last_of(" ");
		if(pos != std::string::npos)
		{ 
			pos++;
			//remove ' '
			arg = arg.substr(pos);
			command = m_input->text.substr(SHELL_PREFIXSIZE, pos);
		}
		int argLen = arg.size();
		m_option = 0;
		//filter all nonmatching
		for(int i = 0 ; i < options.size(); i++)
		{
			const std::string & option = options[i];
			if(option.size() >= argLen && strncmp(arg.c_str(), option.c_str(), argLen) == 0)
			{
				m_options.push_back(SHELL_PREFIX + command + option);
			}
		}
		if(m_options.size())
			overrideInput(m_options[m_option]);	
	}
	else if(m_options.size())
	{
		m_option += m_optionDirection;
		if(m_option < 0) 
			m_option = 0;
		else if(m_option >= m_options.size()) 
			m_option=m_options.size()-1;
		overrideInput(m_options[m_option]);	
	}



}