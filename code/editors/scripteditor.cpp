
#include "../sys.hpp"
#include "scripteditor.hpp"
#include <iostream>


ScriptEditor::ScriptEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{
	m_scriptName = "";
	m_cursorX = m_cursorY = 0;
}

void ScriptEditor::onEnter()
{
	Editor::onEnter();

	LOG("Entering script editor ... ");
	int w, h;
	Engine::GetSize(w, h);

	std::string m_code;
	uint m_codeOffset;

	const std::string & fontname = DEFAULT_FONT;
	Graphics::Font * font = Assets::Load<Graphics::Font>( fontname );
	m_charW = font->charW;
	m_charH = font->charH;
	Assets::Unload<Graphics::Font>(fontname);

	//can scroll horiz 
	const int lineW = w / m_charW * 2;
	//space for toolbar
	const int lineH = h / m_charH - 1;

	m_codeView = new Graphics::TextBox(lineW, lineH, "",fontname);
	m_codeView->x = 0;
	m_codeView->y = menuY(); 
	m_codeView->fillColor = BLACK ;       
	m_codeView->filled = true;
	m_codeView->text = "";
	m_codeView->reload();

	m_cursor = new Graphics::TextBox(1, 1, " ", fontname);
	m_cursor->filled = true;
	m_cursor->fillColor = WHITE ;
	m_cursor->reload();

	reload();

}

void ScriptEditor::onExit()
{
	Assets::Unload<Script>(m_scriptName);
	m_script =  0;

	delete m_codeView;
	App::clear();
	Editor::onExit();
	LOG("Exited script editor");
	if(m_cursor)
	{
		delete m_cursor;
		m_cursor = 0;
	} 
}



void ScriptEditor::onTick()
{
	Engine::ClearScreen(EDITOR_COLOR);

	m_timer += Engine::GetTimeDeltaMs()/1000.0f;
	if (m_timer > 1.0/CURSOR_FLICKER_RATE  ) 
	{
		m_cursor->visible = !m_cursor->visible;
		m_timer = 0;
	}
	if(m_dirty)
	{
		//copy lines to codeview.
		m_codeView->text.clear();
		int line = 0;
		for(int i = m_textOffset; i < m_script->code.size(); i++)
		{
			//if beyond code view 
			if(line >= m_codeView->th)
				break;
			const char c = m_script->code[i];
			if(c == '\n')
			{
				line++;
			}
			m_codeView->text += c;
		}
		m_codeView->refresh();
		m_dirty = false;
	}
	m_cursor->x = m_codeView->x + ( m_cursorX - m_offsetX ) * m_charW;	
	m_cursor->y = m_codeView->y + ( m_cursorY - m_offsetY ) * m_charH;	
	
	m_codeView->draw();
	m_cursor->draw();
}

//
void ScriptEditor::onKey(Key key, bool isDown)
{
	if(isDown)
	{
		switch(key)
		{
			case KEY_UP:
				scrollTextBy(0, -1);
				break;
			case KEY_DOWN:
				scrollTextBy(0, 1);
				break;
			case KEY_LEFT:
				scrollTextBy(-1, 0);
				break;
			case KEY_RIGHT:
				scrollTextBy(1, 0);
				break;
			case KEY_RETURN:
				if (m_cursorPos == m_script->code.size()) 
					m_script->code += '\n';
				else
					m_script->code.insert(m_cursorPos, 1, '\n');
				m_dirty = true;
				m_cursorX = 0;
				scrollTextBy(0, 1);
				break;
			case KEY_TAB:
			default:
				if(KeyPrintable(key))
				{
					//TODO allow horizontal scrolling
					if(m_cursorX < m_codeView->tw-1)
					{
						if (m_cursorPos == m_script->code.size()) 
							m_script->code += key;
						else
							m_script->code.insert(m_cursorPos, 1, key);
						scrollTextBy(1, 0);
					}
					m_dirty = true;
				}
			break;
			case KEY_BACKSPACE:

				if (m_cursorPos > 0)
				{
					scrollTextBy(-1, 0);	
					if(m_cursorPos == m_script->code.size() )
						m_script->code.pop_back();
					else
						m_script->code.erase(m_cursorPos, 1);

					m_dirty = true;
				} 

			break;
		}
	}
}

void ScriptEditor::reload()
{
	Assets::Unload<Script>(m_scriptName);
	m_script = Assets::Load<Script>(m_scriptName);

	m_dirty = true;
	
	m_timer = 0;
	m_offsetY = m_offsetX = 0;
	m_cursorPos = m_textOffset = 0;

}


void ScriptEditor::scrollTextBy(int dx, int dy)
{
	m_dirty = true;
	m_cursorX+=dx;
	m_cursorY+=dy;

	if(m_cursorX < 0)
	{
		m_cursorY--;
		//go to end of line
		m_cursorX = -1;
	}
	else if(m_cursorX >= m_codeView->tw)
	{
		m_offsetX++;
	}
	if(m_cursorY < 0)
	{
		m_cursorY = 0;
	}
	else if(m_cursorY >= m_codeView->th)
	{
		m_offsetY++;
	}

	//set text offset
	int row = 0, col = 0;
	for(m_cursorPos = 0; m_cursorPos < m_script->code.size(); m_cursorPos++)
	{
		if(row == m_cursorY)
		{
			//go to end
			if(m_cursorX == -1)
			{
				m_cursorX = 0;
				while(m_cursorPos < m_script->code.size() && m_script->code[m_cursorPos] != '\n' )
				{
					m_cursorPos++;
					m_cursorX++;
				}
				break;
			} 
			else if(col == m_cursorX)
			{
				break;
			}
		}
		if(m_script->code[m_cursorPos] == '\n' )
		{
			row++;
			col = 0;
		}
		else
		{
			col++;
		}
	}
}

void ScriptEditor::undo()
{ 	
}

void ScriptEditor::redo()
{

}

void ScriptEditor::save()
{
}

void ScriptEditor::setScript(const std::string & name)
{
	m_scriptName = name;

}