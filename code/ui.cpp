#include "ui.h"
#include "graphics.h"
#include "engine.h"
SheetPicker::SheetPicker(const Graphics::Sheet * sheet)
{
	m_sheet = sheet;
	int w, h;
	Engine::GetSize(w, h);

	m_box = { 0, h - m_sheet->h / 2, m_sheet->w * 2, m_sheet->h / 2 };

	m_cursor = {0,0,TILE_W,TILE_H};	
	m_srcLeft = { 0, 0, m_sheet->w, m_sheet->h/2 };
	m_destLeft = { m_box.x, m_box.y, m_sheet->w, m_sheet->h/2 }; 
	
	m_srcRight = { 0, m_sheet->h/2, m_sheet->w, m_sheet->h/2 };
	m_destRight = {m_box.x + m_sheet->w, m_box.y,  m_sheet->w, m_sheet->h/2 }; 
}

SheetPicker::~SheetPicker()
{
}


Rect SheetPicker::selection()
{
	//since texture is half height and twice width, remap back to square sheet
	int x = m_cursor.x;
	int y = m_cursor.y;
	if (x >= m_sheet->w )
	{
		y += m_sheet->h/2;
		x -= m_sheet->w;
	}
	return {x,y , m_cursor.w, m_cursor.h };
}

void SheetPicker::update()
{
	if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
	{
		int mx, my;
		Engine::GetMousePosition(mx, my);
		//get mouse position relative to texture rect
		int rmx = mx-m_box.x;
        int rmy = my-m_box.y;
        if(rmx >= 0 && rmx < m_box.w && rmy >= 0 && rmy < m_box.h)
		{
			//get x y relative to texture
			m_cursor.x = (rmx / m_cursor.w) * m_cursor.w;
			m_cursor.y = (rmy / m_cursor.h) * m_cursor.h;
		}
	}
}
void SheetPicker::draw()
{
	const Rect & worldCursor = 
	{
		m_cursor.x + m_box.x, 
		m_cursor.y + m_box.y, 
		m_cursor.w, 
		m_cursor.h
	};
	Engine::DrawTexture(m_sheet->texture, m_srcLeft, m_destLeft);
	Engine::DrawTexture(m_sheet->texture, m_srcRight, m_destRight);
	Engine::DrawRect(Palette[25], worldCursor, false);

}
