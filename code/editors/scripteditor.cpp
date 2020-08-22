
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
	m_lineOffset = 0;
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

	int lineW = w / m_charW;
	//space for toolbar  
	int lineH = h / m_charH - 1;
	m_lines.resize( lineH );
	m_codeView = new Graphics::TextBox(lineW, lineH, "",fontname);
	m_codeView->x = 0;
	m_codeView->y = menuY(); 
	m_codeView->fillColor = BLACK ;       
	m_codeView->filled = true;
	m_codeView->reload();


	m_dirty = true;
	
	m_script = Assets::Load<Script>(m_scriptName);

	m_timer = 0;

	m_cursor = new Graphics::TextBox(1, 1, " ", fontname);
	m_cursor->filled = true;
	m_cursor->fillColor = WHITE ;
	m_cursor->reload();


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
		int i = 0;
		m_codeView->text.clear();
		for(int line = m_lineOffset; line < m_lines.size(); line++)
		{
			if(i >= m_codeView->th)
				break;

			m_codeView->text += m_lines[i];
			i++;
		}
		m_codeView->refresh();
		m_dirty = false;
	}
	m_cursor->x = m_codeView->x + m_cursorX * m_charW;	
	m_cursor->y = m_codeView->y + m_cursorY * m_charH;	
	
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
			case KEY_RETURN:
				m_lines[m_cursorY] += "\n";
				m_cursorY++;
				m_cursorX = 0;
				if(m_cursorY >= m_codeView->th)
				{
					m_cursorY--;
					m_lineOffset++;
				}
				m_dirty = true;
			break;
			case KEY_TAB:
			default:
				if(KeyPrintable(key))
				{
					std::string & line =m_lines[m_cursorY]; 
					//TODO allow horizontal scrolling
					if(m_cursorX < m_codeView->tw-1)
					{
						if (m_cursorX == line.size()) 
							line += key;
						else
							line.insert(m_cursorX, 1, key);
						m_cursorX++;
					}
					m_dirty = true;
				}
			break;
			case KEY_BACKSPACE:
			{
				std::string & line =m_lines[m_cursorY]; 
				//do not remove \n
				if (line.size() > 1)
				{
					if(m_cursorX > 1)
						m_cursorX--;
					if (m_cursorX>= line.size()-1)
						line.pop_back();
					else
						line.erase(m_cursorX, 1);
				}
				m_dirty = true;

			}
			break;
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