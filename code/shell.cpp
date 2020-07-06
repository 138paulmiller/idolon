#include "shell.h"
#include "engine.h"
#include "graphics.h"
#include "assets.h"
#include <sstream>

Shell::Shell() 
{

}


Shell::~Shell() 
{
}

void Shell::onEnter()
{
	Engine::GetSize(m_w, m_h);

	m_font = Assets::Load<Graphics::Font>(m_fontName );
	m_charW = m_font->charW;
	m_charH = m_font->charH;

	m_cursor = new Graphics::TextBox(1, 1, " ");
	m_cursor->font = m_fontName ;
	m_cursor->filled = true;
	m_cursor->fillColor = { 255, 255, 255, 255 } ;
	m_cursor->reload();

	m_input = new Graphics::TextBox(m_w/m_charW, 1, ">");
	m_input->font = m_fontName ;    
	m_input->reload();

	//used to render lines
	m_buffer = new Graphics::TextBox(m_w/m_charW, (m_h-1)/m_charH, "");
	m_buffer->font = m_fontName ;
	m_buffer->x = 0;
	m_buffer->y = 0;        
	m_buffer->reload();

	m_lineW = m_w / m_charW;
	m_cursorPos = 1; //offset by > 
	m_stashedInput = "";
	m_timer = 0;


	//Maintain buffer input
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

	if (isDown)
	{
		switch (key)
		{
		case KEY_TAB:
			//auto fill ? execute the "auto-complete" command and delegate main to list options by overriding input
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
		case KEY_BACKSPACE:
			//do not remove >
			if (m_input->text.size() > 1)
			{
				if (m_cursorPos >= m_input->text.size())
					m_input->text.pop_back();
				else
					m_input->text.erase(m_cursorPos, 1);
				if(m_cursorPos > 1)
					m_cursorPos--;
			}
			break;
		case KEY_RETURN:

			log(m_input->text);
			if (m_input->text.size() > 1)
				m_command = m_input->text.substr(1);
			m_cursorPos = 1; //reset
			m_input->text = ">";
			break;
		default:
			if (m_cursorPos < m_lineW-1)
			{
				if (m_cursorPos == m_input->text.size()) 
					m_input->text += key;
				else
					m_input->text.insert(m_cursorPos, 1, key);
				m_cursorPos++;
			}
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
	m_buffer->text = msg; 
}
//Can only be called in command
void Shell::log(const std::string & msg)
{
	//split into lines
	std::string line;
	std::istringstream iss;
	iss.str(msg);
	m_buffer->text = ""; //reset buffer. will be rebuilt
	while (std::getline(iss, line))
	{
		m_lines.push_back(msg);
		//move input down, if cannot go down further pop lines off buffer
		m_input->y += m_charH;
		if (m_input->y >= m_h)
		{
			m_input->y = m_h - m_charH;
			m_lines.pop_front();
		}
	}

	for (const std::string& line : m_lines)
	{
		m_buffer->text += line + '\n';
	}

	m_buffer->refresh();
}
