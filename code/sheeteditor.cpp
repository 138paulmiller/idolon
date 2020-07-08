#include "graphics.h"
#include "engine.h"
#include "assets.h"
#include "sheeteditor.h"
#include <iostream>


using namespace Graphics;



void SheetEditor::onEnter()
{
	m_sheet = Assets::Load<Graphics::Sheet>(m_sheetName);
	memset(m_sheet->pixels, 255, m_sheet->w * m_sheet->h * sizeof(Color));
	m_sheet->update();

	addElement( m_sheetPicker = new SheetPicker(m_sheet) );

	int buttonId = addButton(new TextButton("Save", 0, 0, 4, 1));
	getButton(buttonId)->cbClick = [&](){
		Assets::Save(m_sheet);
	};
}

void SheetEditor::onExit()
{
	m_sheetName = "";
}


void SheetEditor::onTick()
{
	Engine::ClearScreen();

	//update 
	int mx, my;
	Engine::GetMousePosition(mx, my);
	const Color &color = Palette[m_colorIndex];
	//draw current tile
	const Rect& tileSrc = m_sheetPicker->selection();
	const Rect & tileDest = { 24, 24, tileSrc.w * m_tileScale, tileSrc.h * m_tileScale }; 	

	int tmx = ((mx-tileDest.x) / m_tileScale) * m_tileScale;
	int tmy = ((my-tileDest.y)/ m_tileScale) * m_tileScale;

	if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
	{
		//get pixel in sheet 
		if (tmx >= 0 && tmx < tileDest.w && tmy >= 0 && tmy < tileDest.h)
		{
			int smx = ( tmx  / m_tileScale) + tileSrc.x;
			int smy = ( tmy  / m_tileScale) + tileSrc.y;
			m_sheet->pixels[m_sheet->w * smy + smx] = color;
			m_sheet->update();
		}
	}
	// draw tile
	Engine::DrawTexture(m_sheet->texture, tileSrc, tileDest);
	//draw pixel brush
	if (tmx >= 0 && tmx < tileDest.w && tmy >= 0 && tmy < tileDest.h)
		Engine::DrawRect(color, { tileDest.x+tmx, tileDest.y+tmy, m_tileScale, m_tileScale}, true);
	

}

//
void SheetEditor::onKey(Key key, bool isDown)
{

}


//Create buttons to resize?
void SheetEditor::setSheet(const std::string& name)
{
	m_sheetName = name;
}

