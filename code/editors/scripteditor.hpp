#pragma once
#include "../game.hpp"
#include "../ui/api.hpp"
#include "../scripting/api.hpp"


//This is the main editor. used to switch to set tilesets, and edit scripts
//Edits the description file
class  ScriptEditor : public Editor 
{
public:
	ScriptEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;
	void redo()override;
	void undo()override;
	void save()override;

	void setScript(const std::string & name);

private:
	//draw line numbers
	void updateTextOffset();
	void runCode();
	void scrollTextBy( int dx, int dy);
	void reload();
	void hide(bool isHidden);

	Script * m_script;
	bool m_scriptRunning;
	std::string m_scriptName;
	int m_cursorX, m_cursorY;
	uint m_cursorPos, m_textOffset;
	//size of code box in chars
	int m_lineW;
	int m_lineH;
	Graphics::TextBox * m_codeBox;
	bool m_dirty ;
//size of char in pixels
	int m_charW;
	int m_charH;
	Graphics::TextBox *m_cursor;
	float m_timer ;

};