#pragma once
#include "../ui/api.hpp"
#include "../game.hpp"

/*
	

*/
class  GameEditor : public Editor 
{
public:
	GameEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , ButtonState state) override;
	void redo()override;
	void undo()override;
	void save()override;

	
	//path to game directory. contains .desc file
	void load(const std::string & gamepath);

private:
	std::string m_gamepath;
	Graphics::TextBox * m_text;
	Game::Desc * desc;
};