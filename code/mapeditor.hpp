#pragma once
#include "sys.hpp"
#include "editor.hpp"

#include <vector>
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
	Graphics::Map * m_map;
	UI::TilePicker * m_tilepicker;
	//DEBUG
	std::vector<Graphics::Sprite*> m_sprites;

	//previous mouse xy
	int m_prevmx,m_prevmy;

	bool m_shift = false;
	//load 4 tiles ! 

};