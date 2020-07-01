#include "shell.h"
#include "engine.h"
#include "graphics.h"
#include "assets.h"
#include <sstream>

namespace
{
	std::string s_fontName = "default";
	Graphics::Font* s_font;
	int s_charW, s_charH;
	int s_w, s_h;
	const int cursorFlickRate = 3;
	Graphics::TextBox * s_cursor, * s_input, * s_buffer;
	std::list<std::string> 	s_lines;
	CommandTable s_commands;
}

namespace Shell
{
	void Startup(const CommandTable & commands)
	{
		s_commands = commands;
		Engine::GetSize(s_w, s_h);
		s_lines.clear();

		s_font = Assets::Load<Graphics::Font>(s_fontName );
		s_charW = s_font->charW;
		s_charH = s_font->charH;

		s_cursor = new Graphics::TextBox(1, 1, " ");
		s_cursor->font = s_fontName ;
		s_cursor->filled = true;
		s_cursor->fillColor = { 255, 255, 255, 255 } ;
		s_cursor->reload();
	
		s_input = new Graphics::TextBox(s_w/s_charW, 1, ">");
		s_input->font = s_fontName ;    
		s_input->reload();

		//used to render lines
		s_buffer = new Graphics::TextBox(s_w/s_charW, (s_h-1)/s_charH, "");
		s_buffer->font = s_fontName ;
		s_buffer->x = 0;
		s_buffer->y = 0;        
		s_buffer->reload();

	}
	
	void Shutdown()
	{
		s_lines.clear();
	}
	
	//Can only be called in command
	void Log(const std::string & msg)
	{
		//split into lines
		std::string line;
		std::istringstream iss;
		iss.str(msg);
		s_buffer->text = ""; //reset buffer. will be rebuilt
		while (std::getline(iss, line))
		{
			s_lines.push_back(msg);
			//move input down, if cannot go down further pop lines off buffer
			s_input->y += s_charH;
			if (s_input->y >= s_h)
			{
				s_input->y = s_h - s_charH;
				s_lines.pop_front();
			}
		}
		for (std::string& line : s_lines)
		{
			s_buffer->text += line + '\n';
		}

		s_buffer->refresh();
	}

	void Run()
	{
		int lineW = s_w / s_charW;
		int cursorPos = 1; //offset by > 
		std::string stashedInput = "";
		Engine::SetKeyEcho(true);
		Engine::SetKeyHandler(
			[&](Key key, bool isDown) 
			{
				if (isDown)
				{
					switch (key)
					{
					case KEY_LEFT:
						if(cursorPos > 1) 
							cursorPos--;					
						break;
					case KEY_RIGHT:
						if(cursorPos < s_input->text.size()) 
							cursorPos++;					
						break;
					case KEY_UP:
					case KEY_DOWN:
						break;
					case KEY_BACKSPACE:
						//do not remove >
						if (s_input->text.size() > 1)
						{
							if (cursorPos >= s_input->text.size())
								s_input->text.pop_back();
							else
								s_input->text.erase(cursorPos, 1);
							if(cursorPos > 1)
								cursorPos--;
						}
						break;
					case KEY_RETURN:

						Log(s_input->text);
						if (s_input->text.size() > 1)
							Execute(s_input->text.substr(1), s_commands); 
						cursorPos = 1; //reset
						s_input->text = ">";
						break;
					default:
						if (cursorPos < lineW-1)
						{
							if (cursorPos == s_input->text.size()) 
								s_input->text += key;
							else
								s_input->text.insert(cursorPos, 1, key);
							cursorPos++;
						}
					}
					s_input->refresh();
				}
			}
		);

		float timer = 0;
		while (Engine::Run())
		{
			timer += Engine::GetTimeDeltaMs()/1000.0f;
			if (timer > 1.0/cursorFlickRate) 
			{
				s_cursor->visible = !s_cursor->visible;
				timer = 0;
			}
		
			s_cursor->x = cursorPos * s_charW;	
			s_cursor->y = s_input->y;	

			s_buffer->draw();
			s_input->draw();
			s_cursor->draw();

		}
		Engine::SetKeyEcho(false);
	}
}