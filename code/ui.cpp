#include "ui.h"
#include "graphics.h"
#include "engine.h"

Button::Button()
:m_rect{0,0,0,0}
{}
const Rect & Button::rect()
{
	return m_rect;
}

UI::~UI()
{
	for(Element * elem : m_elements)
	{
		if(elem)
			delete elem;
	}
	for(Button * button : m_buttons)
	{
		if(button)
			delete button;
	}
}

void UI::update()
{
	for(Element * elem : m_elements)
	{
		if(elem)
			elem->onUpdate();
	}
	for(Button * button : m_buttons)
	{
		if(button)
		{
			button->onUpdate();
			int mx, my;
			Engine::GetMousePosition(mx, my);
			const Rect & mrect = {mx, my, 1,1}; 
			if(button->rect().intersects(mrect))
			{
				if( Engine::GetMouseButtonState(MouseButton::MOUSEBUTTON_LEFT) == BUTTON_DOWN)
				{
				
					button->onClick();
					if(button->cbClick)
						button->cbClick();
				}	
				else
				{
					if(button->cbHover)
						button->cbHover();
			
					button->onHover();
				}
				
			}

		}

	}
}
void UI::draw()
{	
	for(Element * elem : m_elements)
	{
		if(elem)
			elem->onDraw();
	}
	for(Button * button : m_buttons)
	{
		if(button)
			button->onDraw();
	}
}

int UI::addElement(Element * element)
{
	int idx = m_elements.size();
	m_elements.push_back(element);
	return idx;
}

int UI::addButton(Button * button)
{

	int idx = m_buttons.size();
	m_buttons.push_back(button);
	return idx;
}
Button * UI::getButton(int idx)
{
	if(idx < m_buttons.size())
		return m_buttons[idx];
	return 0;
}

Element * UI::getElement(int idx)
{
	if(idx < m_elements.size())
		return m_elements[idx];
	return 0;
}
void UI::removeButton(int idx)
{

	if(idx >= m_buttons.size()) return;
	delete m_buttons[idx];
	m_buttons[idx] = 0;
}

void UI::removeElement(int idx)
{
	if(idx >= m_elements.size()) return;
	delete m_elements[idx];
	m_elements[idx] = 0;

}

TextButton::TextButton(const std::string & text, int x, int y, int tw, int th)
{
	
	hoverTextColor = Palette[7];
	hoverColor = Palette[5];

	textColor = Palette[6];
	color=  Palette[4];

	m_isDirty = true;
	
	m_textbox = new Graphics::TextBox(tw, th, text);
	m_textbox->x = x;
	m_textbox->y = y;
	m_textbox->textColor = textColor;
	m_textbox->fillColor = color;
	m_textbox->filled = true;
	m_textbox->reload();

	Button::m_rect = { m_textbox->x, m_textbox->y, m_textbox->w, m_textbox->h };
	
}
TextButton::~TextButton()
{
	delete m_textbox;
}

void TextButton::onUpdate() 
{
	if(m_isDirty)
	{

		m_textbox->textColor = textColor;
		m_textbox->fillColor = color;
		m_textbox->refresh();
		m_isDirty = false;

	}
}

void TextButton::onDraw() 
{
	m_textbox->draw();
}

void TextButton::onClick()
{
	m_textbox->refresh();	
	m_isDirty = true;

}


void TextButton::onHover()
{
	m_textbox->textColor = hoverTextColor;
	m_textbox->fillColor = hoverColor;
	m_textbox->refresh();
	m_isDirty = true;
}


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

void SheetPicker::onUpdate()
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
void SheetPicker::onDraw()
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
