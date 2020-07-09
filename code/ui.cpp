#include "ui.h"
#include "graphics.h"
#include "engine.h"


namespace UI
{
	Button::Button()
	:m_rect{0,0,0,0}
	{
	}
	
	const Rect & Button::rect()
	{
		return m_rect;
	}

	App::~App()
	{
		clear();
	}

	void App::clear()
	{

		for(Widget * widget : m_widgets)
		{
			if(widget)
				delete widget;
		}
		for(Button * button : m_buttons)
		{
			if(button)
				delete button;
		}
		m_widgets.clear();
		m_buttons.clear();
	}

	void App::update()
	{
		for(Widget * widget : m_widgets)
		{
			if(widget)
				widget->onUpdate();
		}
		for(Button * button : m_buttons)
		{
			if(button)
			{
				button->onUpdate();
				int mx, my;
				Engine::GetMousePosition(mx, my);
				if(button->rect().intersects({mx, my, 1,1}))
				{
					if( Engine::GetMouseButtonState(MouseButton::MOUSEBUTTON_LEFT) 
						== BUTTON_DOWN)
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
	void App::draw()
	{	
		for(Widget * widget : m_widgets)
		{
			if(widget)
				widget->onDraw();
		}
		for(Button * button : m_buttons)
		{
			if(button)
				button->onDraw();
		}
	}

	int App::addWidget(Widget * widget)
	{
		printf("Adding Widget");
		int idx = m_widgets.size();
		m_widgets.push_back(widget);
		return idx;
	}

	int App::addButton(Button * button)
	{

		int idx = m_buttons.size();
		m_buttons.push_back(button);
		return idx;
	}
	Button * App::getButton(int idx)
	{
		if(idx < m_buttons.size())
			return m_buttons[idx];
		return 0;
	}

	Widget * App::getWidget(int idx)
	{
		if(idx < m_widgets.size())
			return m_widgets[idx];
		return 0;
	}
	void App::removeButton(int idx)
	{

		if(idx >= m_buttons.size()) return;
		delete m_buttons[idx];
		m_buttons[idx] = 0;
	}

	void App::removeWidget(int idx)
	{
		if(idx >= m_widgets.size()) return;
		delete m_widgets[idx];
		m_widgets[idx] = 0;

	}

	TextButton::TextButton(const std::string & text, int x, int y, int tw, int th)
	{
		hoverTextColor = WHITE;
		hoverColor = Palette[25];

		textColor = WHITE;
		color=  BLACK;

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

	ColorPicker::ColorPicker()
	{
		static int colorPickerId = 0;
		int w, h;
		//Temp sheet
		m_colorSize = 8; //8x8 
		Engine::GetSize(w, h);

		const std::string& name = "ColorPicker" + std::to_string(colorPickerId++);
		m_sheet = new Graphics::Sheet(name, 2, PaletteCount/2);
		memcpy(m_sheet->pixels, Palette, PaletteCount * sizeof(Color));
		m_sheet->update();

		const int border = m_colorSize / 2;
		const int x = w - m_sheet->w * m_colorSize - border;
		const int y = border;

		m_color = Palette[0];
		m_src = { 0, 0, m_sheet->w, m_sheet->h };
		m_dest = { 
			x,y, 
			m_sheet->w * m_colorSize, 
			m_sheet->h * m_colorSize 
		};
		
		m_border = { 
			m_dest.x - 1,
			m_dest.y - 1, 
			m_dest.w + 2, 
			m_dest.h + 2
		};
		m_cursor = {0,0,m_colorSize,m_colorSize};	
		

	}
	ColorPicker::~ColorPicker()
	{
		delete m_sheet;
	}

	void ColorPicker::onUpdate()
	{
		if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
		{
			int mx, my;
			Engine::GetMousePosition(mx, my);
			//get mouse position relative to texture rect
			int rmx = mx-m_dest.x;
			int rmy = my-m_dest.y;
			if(rmx >= 0 && rmx < m_dest.w && rmy >= 0 && rmy < m_dest.h)
			{
				//get x y relative to texture
				m_cursor.x = (rmx / m_cursor.w) * m_cursor.w;
				m_cursor.y = (rmy / m_cursor.h) * m_cursor.h;
			}
		}
	}

	void ColorPicker::onDraw()
	{
		const Rect & worldCursor = 
		{
			m_cursor.x + m_dest.x, 
			m_cursor.y + m_dest.y, 
			m_cursor.w, 
			m_cursor.h
		};
		Engine::DrawTexture(m_sheet->texture, m_src, m_dest);
		Engine::DrawRect(CURSOR_COLOR, worldCursor, false);
		Engine::DrawRect(BORDER_COLOR, m_border, false);
	}

	Color ColorPicker::color()
	{
		//get cursor x, y then remap to 1 d array
		//since texture is half height and twice width, remap back to square sheet
		int x = m_cursor.x / m_cursor.w;
		int y = m_cursor.y / m_cursor.h;
		return Palette[m_sheet->w * y + x];
	}

	SheetPicker::SheetPicker(const Graphics::Sheet * sheet)
	{
		setSheet(sheet);
	}

	SheetPicker::~SheetPicker()
	{
	}

	Rect SheetPicker::selection()
	{

		if(!m_sheet) return {-1,-1,0,0};
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
	void SheetPicker::setSheet(const Graphics::Sheet * sheet)
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

	void SheetPicker::onDraw()
	{
		if(!m_sheet) return;
		const Rect & worldCursor = 
		{
			m_cursor.x + m_box.x, 
			m_cursor.y + m_box.y, 
			m_cursor.w, 
			m_cursor.h
		};
		Engine::DrawTexture(m_sheet->texture, m_srcLeft, m_destLeft);
		Engine::DrawTexture(m_sheet->texture, m_srcRight, m_destRight);
		Engine::DrawRect(CURSOR_COLOR, worldCursor, false);

	}

}