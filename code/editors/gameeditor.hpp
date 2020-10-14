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

	void setGame(const std::string & gameHeaderPath);

private:

	//if a new row is added, then must offset all text inputs
	void redrawHeaderData();
	void requestRedrawHeader();

	void setName( const std::string &name );

	void openTilesetInputs( );
	void closeTilesetInputs( );
	
	//managed to allow redraw
	std::vector<int> m_headerButtons;
	
	Game::Header * m_header;
	//may not be loaded. only load when running game
	Game::Cartridge * m_cart;

	bool m_requiresRedraw;
};