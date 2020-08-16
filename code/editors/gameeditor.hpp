#pragma once
#include "../ui/api.hpp"
#include "../game.hpp"


//This is the main editor. used to switch to set tilesets, and edit scripts
//Edits the description file
class  GameEditor : public Editor 
{
public:
	GameEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;
	void redo()override;
	void undo()override;
	void save()override;
private:
	Graphics::TextBox * m_text;
	Game::Desc * desc;
};