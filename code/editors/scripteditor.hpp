#pragma once
#include "../game.hpp"
#include "../ui/api.hpp"
#include "../scripting/api.hpp"
#include <FileWatch.hpp>


//This is the main editor. used to switch to set tilesets, and edit scripts
//Edits the description file
class  ScriptEditor : public Editor 
{
public:
	ScriptEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , ButtonState state) override;
	void redo()override;
	void undo()override;
	void save()override;

	void setScript(const std::string & name);

private:
	//draw line numbers
	void runCode();
	void reload();
	void hide(bool isHidden);
	UI::TextScrollArea * m_codeArea;
	Script * m_script;
	bool m_scriptRunning;
	std::string m_scriptName;
	//size of code box in chars
	bool m_initialized;

};