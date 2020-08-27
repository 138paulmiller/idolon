
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
	m_lineW = w / m_charW;
	//space for toolbar
	m_lineH = h / m_charH;

	//create highlight box
	m_codeBox = new Graphics::TextBox(m_lineW * 2, m_lineH, "",fontname);
	m_codeBox->x = m_charW;
	m_codeBox->y = menuY(); 
	m_codeBox->textColor = WHITE;
	m_codeBox->fillColor = BLACK;       
	m_codeBox->filled = false;
	m_codeBox->text = "";
	m_codeBox->reload();

	//only draw text that fits within screen
	m_codeBox->w = m_codeBox->view.w = w; 

	m_cursor = new Graphics::TextBox(1, 1, " ", fontname);
	m_cursor->filled = true;
	m_cursor->fillColor = WHITE ;
	m_cursor->reload();

	reload();

	addTool("RUN", [&](){
		m_script->compile();
		Eval::Execute(m_script->code);
	}, false);

}

void ScriptEditor::onExit()
{
	Assets::Unload<Script>(m_scriptName);
	m_script =  0;

	delete m_codeBox;
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

	int mx, my;
	Engine::GetMousePosition(mx, my);
	if ( Engine::GetMouseButtonState(MOUSEBUTTON_LEFT)== BUTTON_DOWN)
	{
		//relative to textbox
		const int tx = mx - m_codeBox->x;
		const int ty = my - m_codeBox->y;
		if(tx >= 0 && ty >= 0 )
		{
			m_cursorX = tx / m_charW ;
			m_cursorY = ty / m_charH ;
			updateTextOffset();				
		}
	}
	//else if ( Engine::GetMouseButtonState(MOUSEBUTTON_LEFT)== BUTTON_HOLD)
	//highlight text 

	m_timer += Engine::GetTimeDeltaMs()/1000.0f;
	if (m_timer > 1.0/CURSOR_FLICKER_RATE  ) 
	{
		m_cursor->visible = !m_cursor->visible;
		m_timer = 0;
	}
	if(m_dirty)
	{
		//copy lines to codeview.
		m_codeBox->text.clear();
		int line = 0;
		for(int i = m_textOffset; i < m_script->code.size(); i++)
		{
			//if beyond code view 
			if(line >= m_codeBox->th)
				break;
			const char c = m_script->code[i];
			if(c == '\n')
			{
				line++;
			}
			m_codeBox->text += c;
		}
		m_codeBox->refresh();
		m_dirty = false;
	}
	m_cursor->x = m_codeBox->x + ( m_cursorX * m_charW - m_codeBox->view.x );	
	m_cursor->y = m_codeBox->y + ( m_cursorY * m_charH - m_codeBox->view.y );	
	
	m_codeBox->draw();
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
					if(m_cursorX < m_codeBox->tw-1)
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
	m_codeBox->view.x = m_codeBox->view.y = 0;
	m_cursorPos = m_textOffset = 0;

}


void ScriptEditor::scrollTextBy(int dx, int dy)
{
	m_cursorX+=dx;
	m_cursorY+=dy;

	if(m_cursorX < 0)
	{
		m_cursorY--;
		//go to end of line
		m_cursorX = -1;
	}
	
	if(m_cursorY < 0)
	{
		m_cursorY = 0;
	}
	
	//-2 allow to see cursor
	m_codeBox->view.x = Max(0, m_cursorX - (m_lineW-2)) * m_charW;
	m_codeBox->view.y = Max(0, m_cursorY - m_lineH) * m_charH;

	updateTextOffset();
}

void ScriptEditor::updateTextOffset()
{
	m_dirty = true;

	//set text offset
	int cx = m_cursorX;
	int cy = m_cursorY;
	m_cursorX = 0;
	m_cursorY = 0;
	
	for(m_cursorPos = 0; m_cursorPos < m_script->code.size(); m_cursorPos++)
	{
		if(m_cursorY == cy)
		{

			//get as close to cursor X as possible
			while(m_cursorPos < m_script->code.size() 
				&& (cx == -1 || m_cursorX < cx )
				&& m_script->code[m_cursorPos] != '\n' )
			{
				m_cursorPos++;
				m_cursorX++;
			}
			break;
		}
		m_cursorX++;
		if(m_script->code[m_cursorPos] == '\n' )
		{
			m_cursorY++;
			m_cursorX = 0;
		}
	}

}

void ScriptEditor::undo()
{ 	
}

void ScriptEditor::redo()
{
	//todo revision stack!
}

void ScriptEditor::save()
{
	Assets::Save<Script>(m_script);
}

void ScriptEditor::setScript(const std::string & name)
{
	m_scriptName = name;

}