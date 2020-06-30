#include "shell.h"
#include "engine.h"
#include "graphics.h"
#include "assets.h"

namespace
{
	std::string s_fontName = "default";
	Graphics::Font* s_font;
	int s_charW, s_charH;
	int s_w, s_h;
	const int cursorFlickRate = 3;
	Graphics::TextBox * s_cursor, * s_input, * s_buffer;
	std::list<std::string> 	s_lines;
}

namespace Shell
{
	void Startup()
	{
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

	void ExecuteCommand(const std::string & command)
	{
		printf("\n%s", command.c_str());
		//push resutl to lines and update buffer as well
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
				printf("%c\n", key);
				if (isDown)
				{
					char sym = key;
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
						if (cursorPos > 0)
						{
							if (cursorPos > 1 && cursorPos >= s_input->text.size())
							{
								cursorPos--;
								s_input->text.pop_back();
							}
							else
								s_input->text.erase(cursorPos, 1);
						}
						break;
					case KEY_RETURN:
						s_input->y += s_charH;
						if (s_input->y >= s_h -s_charH)
						{
							s_input->y = s_h-s_charH;
							s_lines.pop_front();
						}
						
						s_lines.push_back(s_input->text);
						ExecuteCommand(s_input->text.substr(1)); 
						s_buffer->text = "";
						for (std::string& line : s_lines)
						{
							s_buffer->text += line + '\n';
						}
						s_input->text = ">";
						s_buffer->refresh();
						cursorPos = 1;
						break;
					default:
						//if text can fit in line!
						if (cursorPos < lineW)
						{
							//toupper
							if (sym >= 'a' && sym <= 'z')
								sym = sym - ('a' - 'A');
							if (cursorPos == s_input->text.size()) {
								cursorPos++;
								s_input->text += sym;
							}
							else
							{

								//place one char at cursor pos
								s_input->text.insert(cursorPos, 1, sym);
								cursorPos++;
							}
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