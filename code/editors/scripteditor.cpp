
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
	m_initialized = false;
}

void ScriptEditor::onEnter()
{

	Editor::onEnter();

	LOG("Entering script editor ... ");
	int w, h;
	Engine::GetSize(w, h);

	uint m_codeOffset;

	//can scroll horiz 
	//space for toolbar
	h = (h - controlY());
	int col = 8;
	m_codeArea = new UI::TextScrollArea(col, controlY(), w, h, DEFAULT_FONT);
	
	
	addWidget(m_codeArea);
	reload();

	m_scriptRunning = false;

	addTool("RUN", [&](){
		runCode();
	}, false);

	m_initialized = true;

}

void ScriptEditor::onExit()
{
	Assets::Unload<Script>(m_scriptName);
	m_script =  0;
	m_codeArea = 0;
	
	App::clear();
	Editor::onExit();
	Eval::Shutdown();
	
	LOG("Exited script editor");

	m_initialized = false;
}



void ScriptEditor::onTick()
{

	hide(m_scriptRunning  );

	if ( m_scriptRunning ) 
	{ 
		TypedArg ret;
		m_script->call( GAME_API_UPDATE, ret );
		m_scriptRunning = ret.type == ARG_NONE ||  ret.value.i != 0;
		//reset 
		Runtime::Step();
		return;
	}

	Engine::ClearScreen(EDITOR_COLOR);

}

//
void ScriptEditor::onKey(Key key, ButtonState state)
{
	if (state != BUTTON_RELEASE)
	{
		if (m_scriptRunning)
		{
			switch (key)
			{
			case KEY_ESCAPE:
				m_scriptRunning = false;
				break;
			default:
				break;

			}
			return;
		}
	}
	if (m_codeArea->handleKey(key, state))
	{
		m_script->code = m_codeArea->getText();
	}
}

void ScriptEditor::reload()
{
	Assets::Unload<Script>(m_scriptName);
	m_script = Assets::Load<Script>(m_scriptName);
	
	m_codeArea->setText(m_script->code);
	m_codeArea->resetCursor();

}


void ScriptEditor::runCode()
{
	if (!m_initialized) return; //on enter do nothing 


	Runtime::Quit();
	Eval::Reset(m_script->lang);

	//set input handler. escape to resume
	m_script->compile();
	Engine::PushKeyHandler( 
		[&] ( const Key &key, ButtonState state)
		{
			TypedArg ret;
			m_script->call( GAME_API_ONKEY, ret, { TypedArg( key ), TypedArg(state) } );

			if ( (ret.type != ARG_NONE && ret.value.i == 0 ) || (state && key == KEY_ESCAPE) )
			{
				m_scriptRunning  = false;

				Engine::PopKeyHandler();
			}
		} 
	);
	
	m_scriptRunning  = true;
	
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

void ScriptEditor::hide(bool isHidden)
{
	hideControl( isHidden);
	hideTools( isHidden);
	m_codeArea->hide(isHidden);
}