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
	void onRedo()override;
	void onUndo()override;
	void onSave()override;

	void setGame(const std::string & gameHeaderPath);

private:

	void package();

	//if a new row is added, then must offset all text inputs
	void redrawHeaderData();
	void requestRedrawHeader();

	void setName( const std::string &name );
	void AddComboBox( const std::string &labelText, std::vector<std::string> &names, int x, int y  );


	std::vector<Graphics::TextBox*> m_labels;
	
	Game::Header * m_header;
	//may not be loaded. only load when running game
	Game::Cartridge * m_cart;

	bool m_requiresRedraw;
};