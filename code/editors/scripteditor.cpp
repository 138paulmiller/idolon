
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

	LOG("Entering script editor ... ");
	int w, h;
	Engine::GetSize(w, h);

	std::string m_code;
	uint m_codeOffset;

	const std::string & fontname = DEFAULT_FONT;
	Graphics::Font * font = Assets::Load<Graphics::Font>( fontname );
	int charW = font->charW;
	int charH = font->charH;
	Assets::Unload<Graphics::Font>(fontname);

	int lineW = w / charW;
	//space for toolbar  
	int lineH = h / charH - 1;

	m_codeView = new Graphics::TextBox(lineW, lineH, "",fontname);
	m_codeView->x = 0;
	m_codeView->y = 0; 
	m_codeView->fillColor = BLACK ;       
	m_codeView->filled = true;
	m_codeView->reload();


	m_dirty = true;
	
	m_script = Assets::Load<Script>(m_scriptName);

	Editor::onEnter();



}

void ScriptEditor::onExit()
{
	Assets::Unload<Script>(m_scriptName);
	m_script =  0;

	delete m_codeView;
	App::clear();
	Editor::onExit();
	LOG("Exited script editor");

}



void ScriptEditor::onTick()
{

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
		Engine::ClearScreen(EDITOR_COLOR);
		m_codeView->refresh();
		m_codeView->draw();
		m_dirty = false;
	}
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
					if(m_cursorX < m_codeView->th)
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