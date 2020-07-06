#include "graphics.h"
#include "engine.h"
#include "assets.h"
#include "editsheet.h"
#include <iostream>


using namespace Graphics;



void EditSheet::onEnter()
{
	//if sheet exists load, else create
	//Create an overlay to see potential changes. Committing this changes replaces current ui texture.
	m_sheet = Assets::Load<Graphics::Sheet>(m_sheetName);
	m_cursor = {0,0,TILE_W* m_sheetScale,TILE_H* m_sheetScale };
}

void EditSheet::onExit()
{
	Assets::Unload(m_sheetName);
	m_sheetName = "";
}


void EditSheet::onTick()
{

	const int boxX = 0;
	const int boxY = TILE_H / 4 * m_sheetScale;
	Engine::AlignMouse(m_sheetScale, m_sheetScale);
	Engine::ClearScreen();

	const Color &color = {255, 255, 0, 0 };
	const Rect & src = { 0, 0, m_sheet->w, m_sheet->h };
	const Rect & dest = { boxX, boxY, m_sheet->w* m_sheetScale, m_sheet->h* m_sheetScale }; 

	int mx,my;
	Engine::GetMousePosition(mx, my);
	
	if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
	{
		//get mouse position relative to texture rect
		int rmx = mx-dest.x;
        int rmy = my-dest.y;
        if(rmx >= 0 && rmx < dest.w && rmy >= 0 && rmy < dest.h)
		{
			//get x y relative to texture
			m_cursor.x = (rmx / m_cursor.w) * m_cursor.w;
			m_cursor.y = (rmy / m_cursor.h) * m_cursor.h;
		}
	}
	 
	Engine::DrawTexture(m_sheet->texture, src, dest);
	const Rect & worldCursor = 
	{
		m_cursor.x + dest.x, 
		m_cursor.y + dest.y, 
		m_cursor.w, 
		m_cursor.h
	};
	Engine::DrawRect({255, 120, 120, 120}, worldCursor, false);

	//TODO - draw entire sheet with cursor rect. 
	// Then render in an edit viewport the selcted tile 
	
	//draw pixel brush
	if(mx >= dest.x && mx < dest.x+dest.w && my >= dest.y && my < dest.y+dest.h)
		Engine::DrawRect(color, { mx, my, m_sheetScale, m_sheetScale}, true);

	Engine::AlignMouse(1, 1);
}

//
void EditSheet::onKey(Key key, bool isDown)
{

}


//Create buttons to resize?
void EditSheet::setSheet(const std::string& name)
{
	m_sheetName = name;
}