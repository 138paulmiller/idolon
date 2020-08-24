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

	void scrollTextBy( int dx, int dy);
	void reload();
	//commit code changes to the script
	void commit();
	//code is offloaded into an array of strings. on commit the script cdoe is copied back over and recompiled
	Script * m_script;
	std::string m_scriptName;
	int m_cursorX, m_cursorY;
	uint m_cursorPos, m_textOffset;
	uint m_offsetX, m_offsetY;
	Graphics::TextBox * m_codeView;
	bool m_dirty ;

	int m_charW;
	int m_charH;
	Graphics::TextBox *m_cursor;
	float m_timer ;
};