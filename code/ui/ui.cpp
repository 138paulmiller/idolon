

#include "ui.hpp"

#define DEFAULT_COLOR_TEXT WHITE
#define DEFAULT_COLOR_FILL  BLACK
#define DEFAULT_COLOR_CLICK Palette[26] 
#define DEFAULT_COLOR_HOVER Palette[25]


namespace UI
{
	//////////////////////////////////////////////////////////////////////////////////
	Button::Button()
	:m_rect{0,0,0,0}
	{
		m_isDown = false;
		m_isHover = false;
		m_isDirty = true;
	}
	void Button::onClick()
	{
		m_isDown = true;
		m_isHover = false;

		m_isDirty = true;
	}
	
	bool Button::isDown()
	{
		return m_isDown;
	}
	
	bool Button::isDirty()
	{
		return m_isDirty;
	}
	void Button::onReset()
	{
		m_color = fillColor;
		m_isHover = false;
		m_isDown = m_isDown && sticky;
		m_isDirty = true;
	}
	void Button::onUpdate() 
	{
		if(	!m_isDirty) return;
		if ( m_isDown )
		{
			m_color = clickColor;
		}
		else if(m_isHover)
		{
			m_color = hoverColor;
		}
		else
		{
			m_color = fillColor;
		}

		m_isDirty = false;
	}
	void Button::onHover()
	{
		if(!m_isHover)
		{
			m_isHover = true;
			m_isDirty = true;
		}
	}
	void Button::reset()
	{
		m_isDown = false;
		m_isHover = false;
		m_isDirty = true;
		onReset();
	}
	const Rect & Button::rect()
	{
		return m_rect;
	}
	void Button::click()
	{
		onClick();
		if(cbClick)
			cbClick();
	}

	//////////////////////////////////////////////////////////////////////////////////
	
	App::App()
	{
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

	void App::signal(AppCode code)
	{
		m_status = code;
	}
	
	AppCode App::status()
	{
		return m_status ;
	}
	
	void App::update()
	{

		for(Widget * widget : m_widgets)
		{
			if(widget) widget->onUpdate();
		}

		for(Button * button : m_buttons)
		{
			if(button)
			{
				button->onReset();

				int mx, my;
				Engine::GetMousePosition(mx, my);
				if(button->rect().intersects({mx, my, 1,1}))
				{
					if( Engine::GetMouseButtonState(MouseButton::MOUSEBUTTON_LEFT) == BUTTON_DOWN)
					{
						button->click();
					}	
					else
					{
						if(button->cbHover)
							button->cbHover();
						button->onHover();
					}
				}
				button->onUpdate();
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
	int App::getButtonCount()
	{
		return m_buttons.size();
	}
	
	int App::getWidgetCount()
	{
		return m_widgets.size();
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
	
	//////////////////////////////////////////////////////////////////////////////////
	TextButton::TextButton(const std::string & text, int x, int y, int tw, int th, const std::string & font)
	{
		textColor  = DEFAULT_COLOR_TEXT ;
		hoverColor = DEFAULT_COLOR_HOVER ;		
		clickColor = DEFAULT_COLOR_CLICK;
		fillColor  = DEFAULT_COLOR_FILL;
		sticky = false;
		m_textbox = new Graphics::TextBox(tw, th, text, font);
		m_textbox->x = x;
		m_textbox->y = y;
		m_textbox->borderX = 2;
		m_textbox->textColor = textColor;
		m_textbox->fillColor = fillColor;
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
		bool update = Button::isDirty();
		Button::onUpdate();
		if ( update )
		{
			m_textbox->fillColor = Button::m_color;
			m_textbox->refresh();
		}
	}

	void TextButton::onDraw() 
	{
		m_textbox->draw();
	}
	
	void TextButton::setFont( const std::string& font )
	{
		m_textbox->font = font;
		m_textbox->reload();	
	}

	void TextButton::setText(const std::string & text) 
	{
		m_textbox->text = text;
		m_textbox->refresh();	

	}
	//----------------------------- Input -----------------------------------------------
	TextInput::TextInput(const std::string & text, int x, int y, int tw, int th, const std::string & font )
		:TextButton(text, x, y, tw, th, font)
		
	{
		this->text = text;
		auto handleKey = [this](Key key, bool isDown)
		{
			if(!isDown) return;

			switch(key)
			{
				case  KEY_ESCAPE:
					this->text = this->m_textprev;
					this->setText(this->text);

					Engine::PopKeyHandler();
					this->reset();
				break;
				case  KEY_TAB:
				case  KEY_RETURN:
					Engine::PopKeyHandler();
					this->reset();
					cbAccept();
				break;
				case KEY_BACKSPACE:
					this->text.pop_back();
					this->setText(this->text);
				break;
				default:
				if(key <= KEY_TILDA)
				{
					if(key >= KEY_SPACE)
					{
						this->text.push_back((char)key);
						this->setText(this->text);
					}
				}
				break;
			}
		};

		this->cbClick = [this, handleKey]()
		{
			Engine::PushKeyHandler(handleKey);
			m_timer = 0.0;
			m_cursorVisible = true;
			m_textprev = this->text;
		};
		this->sticky = true;
	}	

	TextInput::~TextInput()
	{
	}
			//draw cursor
	void TextInput::onDraw() 
	{
		TextButton::onDraw();
		if ( this->isDown() )
		{
			m_timer += Engine::GetTimeDeltaMs()/1000.0f;
			if (m_timer > 1.0/CURSOR_FLICKER_RATE  ) 
			{
				m_cursorVisible = !m_cursorVisible;
				m_timer = 0.0;
			}
			if( m_cursorVisible )
			{
				const int cw = m_textbox->getFont()->charW;
				const int ch = m_textbox->getFont()->charH;
				const int cx = this->m_rect.x + this->text.size() * cw + m_textbox->borderX;
				const int cy = this->m_rect.y + m_textbox->borderY;

				Engine::DrawRect( WHITE, {cx,cy,cw,ch}, true );
			}
		}
	}


/*-------------------------------------- Toolbar -----------------------------------
*/
	Toolbar::Toolbar( App* parent, int x, int y )
		:m_parent(parent), 
		m_x(x),m_y(y),
		m_count(0), m_xoff(0),
		font("default")
	{
		textColor = { 255,255,255,255 };
		hoverColor = DEFAULT_COLOR_HOVER ;		
		clickColor = DEFAULT_COLOR_CLICK;
		fillColor  = DEFAULT_COLOR_FILL;
	}
	
	Toolbar::~Toolbar()
	{
		for ( int id : m_buttonIds )
			m_parent->removeButton( id );
	}

	int  Toolbar::add(const std::string & text, std::function<void()> click, bool sticky )
	{
		m_count++;
		TextButton * textbutton = new TextButton(text, m_x+m_xoff, m_y, text.size(), 1, font);
		m_xoff += textbutton->rect().w;
		
		int buttonId = m_parent->addButton( textbutton );
		textbutton->textColor  = textColor ;
		textbutton->hoverColor = hoverColor;
		textbutton->clickColor = clickColor;
		textbutton->fillColor  = fillColor ;
		textbutton->sticky = sticky ;
		textbutton->setFont(font);
		//perhaps avoidable
		textbutton->cbClick = [this, buttonId, click] () 
		{ 
			//unclick all others
			for ( int i = 0; i < m_buttonIds.size(); i++ )
			{
				int id = m_buttonIds[i];
				if(id != buttonId && m_parent->getButton(id)->isDown())  
					m_parent->getButton(i)->reset();		
			}
			click();
		} ;
		m_buttonIds.push_back(buttonId);
		return m_buttonIds.size()-1;
	}

	void  Toolbar::remove(int id)
	{
		m_parent->removeButton(m_buttonIds[id]);
	}

	Button *  Toolbar::get(int id)
	{
		ASSERT(id >= 0 && id < m_buttonIds.size(), "Toolbar Invalid Id");
		return m_parent->getButton(m_buttonIds[id]);
	}

	void Toolbar::onUpdate()
	{
	}
	void Toolbar::onDraw()
	{}

	
	//////////////////////////////////////////////////////////////////////////////////

	ColorPicker::ColorPicker(int x, int y)
	{
		static int colorPickerId = 0;
		//Temp sheet
		m_colorSize = 8; //8x8 

		const std::string& name = "ColorPicker" + std::to_string(colorPickerId++);
		m_tileset = new Graphics::Tileset(name, 2, PaletteCount/2);
		memcpy(m_tileset->pixels, Palette, PaletteCount * sizeof(Color));
		m_tileset->update();

		m_color = Palette[0];
		m_src = { 0, 0, m_tileset->w, m_tileset->h };
		m_dest = { 
			x,y, 
			m_tileset->w * m_colorSize, 
			m_tileset->h * m_colorSize 
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
		delete m_tileset;
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
		Engine::DrawTexture(m_tileset->texture, m_src, m_dest);
		Engine::DrawRect(CURSOR_COLOR, worldCursor, false);
		Engine::DrawRect(BORDER_COLOR, m_border, false);
	}

	Color ColorPicker::color()
	{
		//get cursor x, y then remap to 1 d array
		//since texture is half height and twice width, remap back to square sheet
		const int x = m_cursor.x / m_cursor.w;
		const int y = m_cursor.y / m_cursor.h;
		const int w = m_tileset->w; //width of sheet is number of colors (single pixel)
		return Palette[w * y + x];
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	const int TilePicker::m_selectionSizes[2][2] = 
	{
		{ TILE_W,   TILE_H },
		{ SPRITE_W, SPRITE_W }
	};

	TilePicker::TilePicker()
		:
		m_scale(2),
		m_tileset(0)
	{
		resizeCursor(m_selectionSizes[0][0], m_selectionSizes[0][0]);
	}

	TilePicker::~TilePicker()
	{
		if(m_tileset)
			Assets::Unload<Graphics::Tileset>(m_tileset->name);
	}

	const Rect & TilePicker::rect()
	{
		return m_box;
	}

	int TilePicker::selectionIndex()
	{
		if ( !m_tileset ) 
			return -1;
		return m_tileset->id(m_cursor);
	}

	Rect TilePicker::selection()
	{
		if(!m_tileset) return {-1,-1,0,0};
		return m_tileset->tile( selectionIndex(), m_cursor.w, m_cursor.h );
	}
	void TilePicker::moveCursor(int dx, int dy)
	{
		int mx = m_cursor.x + (dx * m_cursor.w);
		int my = m_cursor.y + (dy * m_cursor.h);
		if(	mx >= 0 && mx < m_box.w/ m_scale 
		&&  my >= 0 && my < m_box.h/ m_scale)
		{
			m_cursor.x = mx;
			m_cursor.y = my;
		}
	}
	void TilePicker::resizeCursor( int w, int h )
	{
		if ( w > 0 && h > 0 )
		{
			m_cursor.w = w;
			m_cursor.h = h;	
			//snap cursor x y to new w h
			m_cursor.x = (m_cursor.x / w) * w;
			m_cursor.y = (m_cursor.y / h) * h;
		}
	}
	void TilePicker::onUpdate()
	{
		if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
		{
			int mx, my;
			Engine::GetMousePosition(mx, my);
			//get mouse position relative to texture rect
			int rmx = ((mx- m_box.x) / m_scale);
			int rmy = ((my- m_box.y) / m_scale);

			if(rmx >= 0 && rmx < m_box.w && rmy >= 0 && rmy < m_box.h)
			{
				//get x *  y from mouse loca xy
				m_cursor.x = (rmx / m_cursor.w) * m_cursor.w;
				m_cursor.y = (rmy / m_cursor.h) * m_cursor.h;
			}
		}
	}
	void TilePicker::reload(const std::string & tileset)
	{
		if(m_tileset)
			Assets::Unload<Graphics::Tileset>(m_tileset->name);

		m_tileset = Assets::Load<Graphics::Tileset>(tileset);
		
		if ( !m_tileset ) return;

		int w, h;
		Engine::GetSize(w, h);
		
		m_aspect = 2;//w / (m_tileset->w* m_scale);
		const int adjw = ( int ) ( m_tileset->w * m_aspect );
		const int adjh = ( int ) ( m_tileset->w / m_aspect );

		m_cursor = {0,0,TILE_W,TILE_H};	
		m_box = { 0, h - m_tileset->h * m_scale , m_tileset->w * m_scale, m_tileset->h * m_scale};

	}

	void TilePicker::onDraw()
	{
		if(!m_tileset) return;
		const Rect & worldCursor = 
		{
			m_cursor.x * m_scale + m_box.x, 
			m_cursor.y * m_scale + m_box.y, 
			m_cursor.w * m_scale , 
			m_cursor.h * m_scale 
		};

		const Rect& src = { 0, 0, m_tileset->w, m_tileset->h };
		Engine::DrawRect( EDITOR_COLOR, m_box, true);		
		Engine::DrawTexture( m_tileset->texture, src, m_box);

		Engine::DrawRect(CURSOR_COLOR, worldCursor, false);

	}

	void TilePicker::handleKey(Key key, bool isDown)
	{
		if(!isDown) 
			return;
		
		switch(key)
		{
			case KEY_UP:
				moveCursor(0, -1);
				break;
			case KEY_DOWN:
				moveCursor(0, 1);
				break;
			case KEY_RIGHT:
				moveCursor(1, 0);
				break;
			case KEY_LEFT:
				moveCursor(-1, 0);
				break;
			case KEY_1:
				resizeCursor( m_selectionSizes[0][0], m_selectionSizes[0][1] );
				break;
			case KEY_2:
				resizeCursor( m_selectionSizes[1][0], m_selectionSizes[1][1] );
				break;
		}
	}

} // namespace UI