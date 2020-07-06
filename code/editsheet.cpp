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
}

void EditSheet::onExit()
{
	Assets::Unload(m_sheetName);
	m_sheetName = "";
}


void EditSheet::onTick()
{
	Engine::AlignMouse(m_sheetScale, m_sheetScale);
	Engine::ClearScreen();
	int mx,my;
	Engine::GetMousePosition(mx, my);
	//align to pixel size
	if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
	{
		//get mouse position relative to texture rect
	}
	const int boxX = 2;
	const int boxY = 2;
	
	mx-=boxX;
	my-=boxY;
	const Color &color = {255, 255, 0, 0 };
	const Rect & src = { 0, 0, m_sheet->w, m_sheet->h };
	const Rect & dest = { boxX, boxY, m_sheet->w * m_sheetScale, m_sheet->h * m_sheetScale};  
	

	Engine::DrawRect(color, { mx, my, m_sheetScale, m_sheetScale}, true);
	Engine::DrawTexture(m_sheet->texture, src, dest);
	
	//draw "cursor"
	if(mx >= dest.x && mx < dest.x+dest.w && my >= dest.y && my < dest.y+dest.h)
		Engine::DrawRect(color, { mx, my, m_sheetScale, m_sheetScale}, true);

	//draw grid
	const Color &gridColor = {255, 200, 200, 200 };
	for(int x = dest.x; x < dest.x+dest.w; x+=m_gridSize)
	{
		const int sx = x * m_sheetScale ;
		Engine::DrawLine(gridColor, sx, dest.y, sx, dest.y+dest.h );
	}
	for(int y = dest.y; y < dest.y+dest.h; y+=m_gridSize)
	{
		const int sy = y * m_sheetScale ;
		Engine::DrawLine(gridColor, dest.x, sy, dest.x+dest.w, sy );
	}

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