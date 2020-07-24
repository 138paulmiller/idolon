#pragma once
#include "sys.hpp"
#include "editor.hpp"


class  MapEditor : public Editor 
{
public:
	MapEditor();
	void onEnter() override;
	void onExit() override;
	void onTick()override;
	void onKey(Key key , bool isDown) override;

	void redo() override;
	void undo() override;
	void save() override;
	
	//
	void setMap( const std::string& name );
private:
	//by default are all empty
	std::string m_mapName;
	//map contains ref to tilesheet

	//DEBUG
	Graphics::Tileset* m_tileset;
	Graphics::Sprite* m_sprite;

};