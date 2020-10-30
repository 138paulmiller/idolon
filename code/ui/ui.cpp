

#include "ui.hpp"
#include "app.hpp"
#include "../config.hpp"

namespace UI
{

/*--------------------------------------------------------------------------------------
    Button
*/

	Button::Button()
	:m_rect{0,0,0,0}
	,hoverColor(DEFAULT_COLOR_HOVER)
	,clickColor(DEFAULT_COLOR_CLICK)
	,fillColor(DEFAULT_COLOR_FILL)
	{
		m_isDown = false;
		m_isHover = false;
		m_isDirty = true;
		m_prevclick = false;

	}
	void Button::onClick()
	{
		m_isDown = true;
		m_isHover = false;

		m_isDirty = true;
	}
	
	bool Button::isDown() const
	{
		return m_isDown;
	}
	
	bool Button::isDirty()const
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

	void Button::onDraw() 
	{
		Engine::DrawRect( m_color, m_rect, true );
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
	const Rect & Button::rect() const
	{
		return m_rect;
	}
	
	void Button::setRect(int x, int y, int w, int h)
	{
		m_rect = { x,y,w,h };
	}
	
	void Button::leave()
	{
		if ( m_prevclick )
		{
			if(cbLeave)
				cbLeave();
			m_prevclick = false;
		}

	}
	void Button::click()
	{
		onClick();
		if(cbClick)
			cbClick();
		m_prevclick = true;
	}

	

/*--------------------------------------------------------------------------------------
    TextButton
*/
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
		m_textbox->borderX = DEFAULT_TEXT_BORDER;
		m_textbox->borderY = DEFAULT_TEXT_BORDER;

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
			Button::m_rect = { m_textbox->x, m_textbox->y, m_textbox->w, m_textbox->h };
			m_textbox->fillColor = Button::m_color;
			m_textbox->textColor = textColor;
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


	void TextButton::setEscape( bool isEscaped )
	{
		m_textbox->escaped = isEscaped;
	}


/*--------------------------------------------------------------------------------------
    TextInput
*/
	TextInput::TextInput(const std::string & text, int x, int y, int tw, int th, const std::string & font )
		:TextButton(text, x, y, tw, th, font)
		
	{
		this->text = text;
		auto handleKey = [this](Key key, ButtonState state)
		{
			if(state == BUTTON_RELEASE ) return;

			switch(key)
			{
				case  KEY_ESCAPE:
					//set to previous. no change
					this->text = this->m_textprev;
					this->setText(this->text);
					Engine::PopKeyHandler();
					this->reset();
				break;
				case  KEY_TAB:
				case  KEY_RETURN:
					this->cbLeave();
				break;
				case KEY_BACKSPACE:
					if ( !this->text.empty() )
					{
						this->text.pop_back();
						this->setText(this->text);
					}
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
		this->cbLeave = [this]()
		{
			if ( !m_cursorVisible ) return;
			Engine::PopKeyHandler();
			this->reset();
			m_cursorVisible = false;

			this->cbAccept();
		};
		this->sticky = true;
	}	

	TextInput::~TextInput()
	{
		if ( m_cursorVisible )
		{
			Engine::PopKeyHandler();
		}
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
/*-------------------------------------------------------------------
	ScrollBar
*/
	ScrollBar::ScrollBar(  App* parent, int x, int y, int len, bool isHorizontal )
	{
		m_parent = parent;
		m_scroll = 0;

		m_isHoriz = isHorizontal;
		TextButton *upBtn = new TextButton( { 15 }, x, y, 1, 1, DEFAULT_FONT_ICONS );
		setRange( 1, 10 );

		int nx = x, ny=  y;
		if ( m_isHoriz )
		{
			
			nx = x + len - upBtn->rect().w ;
		}
		else 
		{
			ny = y + len - upBtn->rect().h ; 
		}
		
		TextButton *downBtn = new TextButton( { 14 }, nx, ny, 1, 1, DEFAULT_FONT_ICONS );
		Button * bar = new Button();
		bar->fillColor = upBtn->hoverColor;

		m_barBtn  = parent->addButton( bar );
		m_downBtn  = parent->addButton( downBtn );
		m_upBtn  = parent->addButton( upBtn );
		if ( m_isHoriz )
		{
			m_len = downBtn->rect().x - (upBtn->rect().x + upBtn->rect().w) ;
		}
		else
		{
			m_len = downBtn->rect().y - (upBtn->rect().y + upBtn->rect().h) ;
		}

		upBtn->cbClick = [this] () {
			m_scrollDir = -1;
		};
		upBtn->cbLeave = [this] () {
			m_scrollDir = 0;
		};
		downBtn->cbClick = [this] () {
			m_scrollDir = 1;
		};
		downBtn->cbLeave = [this] () {
			m_scrollDir = 0;
		};
		m_scrollDir = 0;

	}
	
	ScrollBar::~ScrollBar()
	{
		m_parent->removeButton( m_upBtn );
		m_parent->removeButton( m_downBtn);
		m_parent->removeButton( m_barBtn );
	}
	void ScrollBar::hide( bool isHidden )
	{
		m_parent->getButton( m_barBtn )->hidden = isHidden;
		m_parent->getButton( m_downBtn )->hidden = isHidden;
		m_parent->getButton( m_upBtn )->hidden = isHidden;
		hidden = isHidden;
	}

	void ScrollBar::onUpdate()
	{		
		scrollBy( m_scrollDir );
	
		const Button *upBtn = m_parent->getButton( m_upBtn );
		//-h*2 to account for up down buttons

		const int bw = upBtn->rect().w;
		const int bh = upBtn->rect().h;
		const float size = ((float)m_step/ m_total) ;
		const int border = 2;
		const int w = ( m_isHoriz ? size * m_len :  bw - border ) ;
		const int h = (!m_isHoriz ? size * m_len :  bh - border ) ;

		const int x = upBtn->rect().x +(  m_isHoriz ? bw + m_scroll/m_total*m_len : border / 2  );
		const int y = upBtn->rect().y +( !m_isHoriz ? bh + m_scroll/m_total*m_len : border / 2  );

		m_parent->getButton(m_barBtn)->setRect(x,y,w,h);

	}
	void ScrollBar::onDraw()
	{
		//draw scroll line 
		Button *upBtn = m_parent->getButton( m_upBtn );
		const int bw = upBtn->rect().w;
		const int bh = upBtn->rect().h;
		const int bx = upBtn->rect().x;
		const int by = upBtn->rect().y;
		const Rect &rect = 
		{ 
			!m_isHoriz ? bx : bx+bw, 
			m_isHoriz ? by : by+bh,
			 m_isHoriz ? m_len : bw,
			!m_isHoriz ? m_len : bh,
		};
		Engine::DrawRect(upBtn->fillColor, rect, true);
	}
	
	void ScrollBar::scrollBy( float d )
	{
		const int prevScroll = m_scroll;
		m_scroll += d;
		//do not overstep
		m_scroll = Clamp( m_scroll, 0, m_total-m_step );
		//if scroll success
		if ( prevScroll != m_scroll && cbScroll )
				cbScroll( d );
		
	}

	void ScrollBar::setRange( int step, int total )
	{
		m_step = step;
		m_total = total;
	}

/*-------------------------------------------------------------------
TextScrollArea
*/
	TextScrollArea::TextScrollArea(App * parent, int x, int y, int w, int h, const std::string & fontname)

	{
		Graphics::Font * font = Assets::Load<Graphics::Font>(fontname);
		m_charW = font->charW;
		m_charH = font->charH;
		Assets::Unload<Graphics::Font>(fontname);
		m_cursorX = m_cursorY = 0;

		m_timer = 0;
		m_textW = w / m_charW ;
		m_textH = h / m_charH;

		//buffer the textbox
		m_textBox = new Graphics::TextBox(m_textW * 2, m_textH, "", fontname);
		m_textBox->x = x;
		m_textBox->y = y;
		m_textBox->textColor = WHITE;
		m_textBox->fillColor = BLACK;
		m_textBox->filled = false;
		m_textBox->text = "";
		m_textBox->reload();

		//only draw text that fits within screen
		m_textBox->view.x = m_textBox->view.y = 0;
		m_textBox->w = m_textBox->view.w = w;
		m_textBox->view.h = h;

		//init cursor
		m_cursor = new Graphics::TextBox(1, 1, " ", fontname);
		m_cursor->filled = true;
		m_cursor->fillColor = WHITE;
		m_cursor->reload();

		const int border = DEFAULT_TEXT_BORDER * 2;
		//scroll bar is roughly 12 pixels
		m_scrollBar = new ScrollBar( parent, w-m_scrollBarWidth, y + border, h - border*2 );
		m_scrollBar->cbScroll = [this] (int step ){ 
			this->scrollPageBy(0, step );
		};

	}

	TextScrollArea::~TextScrollArea()
	{
		delete m_textBox;
		if (m_cursor)
		{
			delete m_cursor;
			m_cursor = 0;
		}
		delete m_scrollBar;
	}
	
	void TextScrollArea::onUpdate() 
	{
		m_scrollBar->setRange( m_textH, m_lines );						
		m_scrollBar->onUpdate();
	}
	
	void TextScrollArea::onDraw()
	{
		if (m_hidden) return;
		//handle mouse 

		int mx, my;
		Engine::GetMousePosition(mx, my);
		if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_CLICK)
		{
			int w, h;
			Engine::GetSize( w, h );
			if (mx >= 0 && my >= 0 && mx < w - m_scrollBarWidth && my < h)
			{
				//relative to textbox
				m_cursorX = (mx - m_textBox->x ) / m_charW;
				m_cursorY = (my - m_textBox->y ) / m_charH + m_textBox->scrolly;
				updateTextOffset();
			}
		}

		m_timer += Engine::GetTimeDeltaMs() / 1000.0f;
		if (m_timer > 1.0 / CURSOR_FLICKER_RATE)
		{
			m_cursor->visible = !m_cursor->visible;
			m_timer = 0;
		}
		m_textBox->draw();
		m_cursor->draw();
		m_scrollBar->onDraw();

	}

	bool TextScrollArea::handleKey(Key key, ButtonState state)
	{
		bool dirty = false;
		if (state != BUTTON_RELEASE)
		{
			switch (key)
			{
			case KEY_UP:
				scrollTextBy(0, -1);
				break;
			case KEY_DOWN:
				scrollTextBy(0, 1);
				break;
			case KEY_LEFT:
				scrollTextBy(-1, 0);
				break;
			case KEY_RIGHT:
				scrollTextBy(1, 0);
				break;
			case KEY_RETURN:
				if (m_cursorPos == m_textBox->text.size())
					m_textBox->text += '\n';
				else
					m_textBox->text.insert(m_cursorPos, 1, '\n');
			
				m_scrollBar->scrollBy( 1 );
				m_lines++;
				dirty = true;
				m_cursorX = 0;
				scrollTextBy(0, 1);
				break;
			case KEY_BACKSPACE:
				if (m_cursorPos > 0)
				{
					scrollTextBy(-1, 0);
					if ( m_cursorPos == m_textBox->text.size() )
					{
						if ( m_textBox->text.back() == '\n' )
						{
							m_scrollBar->scrollBy( -1 );
							m_lines--;
						}
						m_textBox->text.pop_back();
					}
					else
					{
						if ( m_textBox->text[m_cursorPos] == '\n' )
						{
							m_lines--;
						}
						m_textBox->text.erase(m_cursorPos, 1);
					}
					dirty = true;
				}

				break;
			default:
				if (KeyPrintable(key))
				{
					//TODO allow horizontal scrolling
					if (m_cursorX < m_textBox->tw - 1)
					{
						//use spaces
						if (key == KEY_TAB)
						{
							if (m_cursorPos == m_textBox->text.size())
								m_textBox->text += std::string(TAB_SIZE, ' ');
							else
								m_textBox->text.insert(m_cursorPos, TAB_SIZE, ' ');
							scrollTextBy(TAB_SIZE, 0);
						}
						else
						{
							if (m_cursorPos == m_textBox->text.size())
								m_textBox->text += key;
							else
								m_textBox->text.insert(m_cursorPos, 1, key);
							scrollTextBy(1, 0);
						}
					}
					dirty = true;

				}
				break;
			}
		}
		if (dirty)
		{
			m_textBox->refresh();
		}
		return dirty;
	}


	void TextScrollArea::setText(const std::string & text)
	{
		m_lines = 0;
		if (text.size() == 0)
		{
			m_textBox->text = "";
		}
		else
		{
			//copy lines to codeview.
			m_textBox->text = text;
			for ( int i = 0; i < text.size(); i++ )
			{
				if ( text[i] == '\n' )
				{
					m_lines++;
				}
			}
		}

		m_textBox->refresh();
	}

	const std::string & TextScrollArea::getText()
	{
		return m_textBox->text;
	}

	void TextScrollArea::hide(bool isHidden)
	{
		m_scrollBar->hide( isHidden );
		m_hidden = isHidden;
	}

	void TextScrollArea::resetCursor()
	{
		m_timer = 0;
		m_cursorPos = 0;
		m_textBox->scrolly = 0;
		scrollTextBy(0, 0);
	}

	void TextScrollArea::scrollPageBy( int dx, int dy )
	{
		//-2 to see 
		m_textBox->scrolly = Clamp(m_textBox->scrolly + dy, 0, m_lines) ;
		m_textBox->view.x = Clamp(m_textBox->view.x + dx, 0, m_textW - 4) ;
		m_textBox->refresh();
		updateTextOffset();

	}

	void TextScrollArea::scrollTextBy(int dx, int dy)
	{
		m_cursorX += dx;
		m_cursorY += dy;

		//if below 
		if (m_cursorY - m_textBox->scrolly >= m_textH - 1)
		{
			m_textBox->scrolly += m_cursorY - (m_textBox->scrolly + m_textH - 1);
			m_textBox->refresh();

		}
		else if (m_cursorY < m_textBox->scrolly)
		{
			m_textBox->scrolly += m_cursorY - m_textBox->scrolly;
			if (m_textBox->scrolly < 0)
				m_textBox->scrolly = 0;
			m_textBox->refresh();

		}
		if (m_cursorX < 0)
		{
			//snap to end of previous line
			if (m_cursorY > 0)
			{
				m_cursorY--;
				//go to end of line
				m_cursorX = -1;
			}
			else
			{
				m_cursorX = 0;
			}

		}
		//if moving past newline go to next line
		else if (dx > 0 && m_cursorPos && m_cursorPos < m_textBox->text.size() && m_textBox->text[m_cursorPos] == '\n')
		{
			m_cursorX = 0;
			m_cursorY++;
		}

		else if (m_cursorY < 0)
		{
			m_cursorY = 0;
		}

		updateTextOffset();

	}

	void TextScrollArea::updateTextOffset()
	{
		//set text offset
		int cx = m_cursorX;
		int cy = m_cursorY;
		m_cursorX = 0;
		m_cursorY = 0;

		for (m_cursorPos = 0; m_cursorPos < m_textBox->text.size(); m_cursorPos++)
		{
			if (m_cursorY == cy)
			{
				//get as close to cursor X as possible
				while (m_cursorPos < m_textBox->text.size()
					&& (cx == -1 || m_cursorX < cx)
					&& m_textBox->text[m_cursorPos] != '\n')
				{
					m_cursorPos++;
					m_cursorX++;
				}
				//
				break;
			}
			m_cursorX++;
			if (m_textBox->text[m_cursorPos] == '\n')
			{
				m_cursorY++;
				m_cursorX = 0;
			}
		}
	
		//update cursor and code view
		//-4 allow to see cursor
		m_textBox->view.x = Max(0, m_cursorX - (m_textW - 4)) * m_charW;

		m_cursor->x = m_textBox->x + ( m_cursorX * m_charW )- m_textBox->view.x;
		m_cursor->y = m_textBox->y + ( m_cursorY - m_textBox->scrolly ) * m_charH;
	}
/*--------------------------------------------------------------------------------------
	 Toolbar
*/
	Toolbar::Toolbar( App* parent, int x, int y )
		:m_parent(parent), 
		m_x(x),m_y(y),
		m_count(0), m_off(0),
		font("default"),
		leftAlign(true),
		verticalAlign(false)

	{
		textColor = { 255,255,255,255 };
		hoverColor = DEFAULT_COLOR_HOVER ;		
		clickColor = DEFAULT_COLOR_CLICK;
		fillColor  = DEFAULT_COLOR_FILL;
	}
	
	Toolbar::~Toolbar()
	{
//		for ( int id : m_buttonIds )
//			m_parent->removeButton( id );
	}

	int  Toolbar::add(const std::string & text, std::function<void()> click, bool sticky, bool isEscaped )
	{
		if ( !leftAlign && m_count == 0 )
		{
			//if first, must offset x by size of first button
			TextButton * tmp = new TextButton( text, 0,0, text.size(), 1, font );
			m_off = tmp->rect().w;
			delete tmp;
		}
		m_count++;
		const int x = verticalAlign ? m_x : ( leftAlign ? m_x + m_off : m_x - m_off ) ;
		const int y = verticalAlign ? m_y + m_off : m_y;

		TextButton * textbutton = new TextButton( text, x, y, text.size(), 1, font );
		m_off += verticalAlign ? (textbutton->rect().h) : (textbutton->rect().w);
			
		int buttonId = m_parent->addButton( textbutton );
		textbutton->textColor  = textColor ;
		textbutton->hoverColor = hoverColor;
		textbutton->clickColor = clickColor;
		textbutton->fillColor  = fillColor ;
		textbutton->sticky = sticky ;

		textbutton->setEscape( isEscaped );
		textbutton->setFont(font);

		//perhaps avoidable
		textbutton->cbClick = [this, buttonId, click] () 
		{ 
			//unclick all others
			for ( int i = 0; i < m_buttonIds.size(); i++ )
			{
				int id = this->m_buttonIds[i];
				UI::Button *btn = this->m_parent->getButton( id );
				if(id != buttonId && btn->isDown())  
					btn->reset();		
			}
			click();
		} ;
		m_buttonIds.push_back(buttonId);
		return m_buttonIds.size()-1;
		m_prevHidden = hidden;

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
		if(	m_prevHidden != hidden)
		{
			for(int i = 0; i < m_buttonIds.size(); i++)
			{
				get(i)->hidden = hidden;
			}
			m_prevHidden = hidden;
		}
	}
	void Toolbar::onDraw()
	{
	}

	
/*--------------------------------------------------------------------------------------
	 ColorPicker
*/

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
		if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_RELEASE)
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
	


	
/*--------------------------------------------------------------------------------------
	 TilePicker
*/
	const int TilePicker::m_selectionSizes[2][2] = 
	{
		{ TILE_W,   TILE_H },
		{ SPRITE_W, SPRITE_W }
	};

	TilePicker::TilePicker()
		:
		m_scale(1),
		m_tileset(0),
		m_focus(false)
	{
		resizeCursor(m_selectionSizes[0][0], m_selectionSizes[0][0]);

		m_tileIdBox = 0;

	}

	TilePicker::~TilePicker()
	{
		delete m_tileIdBox;
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
		const int leftbtn =Engine::GetMouseButtonState(MOUSEBUTTON_LEFT); 
		if ( leftbtn != BUTTON_RELEASE)
		{
			int mx, my;
			Engine::GetMousePosition(mx, my);
			//get mouse position relative to texture rect
			int rmx = ((mx- m_box.x) / m_scale);
			int rmy = ((my- m_box.y) / m_scale);

			if(rmx >= 0 && rmx < m_box.w && rmy >= 0 && rmy < m_box.h)
			{
				if(leftbtn == BUTTON_CLICK)
				{
					m_focus = true;
				}
				if(m_focus)
				{
					//get x *  y from mouse loca xy
					m_cursor.x = (rmx / m_cursor.w) * m_cursor.w;
					m_cursor.y = (rmy / m_cursor.h) * m_cursor.h;
				}
			}
			else
			{
				//clicked outside
				if(leftbtn == BUTTON_CLICK)
					m_focus = false;
			}
		}

		if ( m_tileIdBox )
		{
			//update id
			const int id = selectionIndex();
			char idText[]  = "000";
			snprintf(idText, 4, "%03d", id);
			m_tileIdBox->setText(idText);
		}

	}
	void TilePicker::reload( const Graphics::Tileset * tileset)
	{
		m_tileset = tileset;
		
		if ( !m_tileset ) return;

		int w, h;
		Engine::GetSize(w, h);
		
		m_aspect = 2;//w / (m_tileset->w* m_scale);
		const int adjw = ( int ) ( m_tileset->w * m_aspect );
		const int adjh = ( int ) ( m_tileset->w / m_aspect );
		const int border = 2;
		m_cursor = {0,0,TILE_W,TILE_H};	
		m_box = { border, h - m_tileset->h * m_scale - border , m_tileset->w * m_scale, m_tileset->h * m_scale};

				const std::string &fontName = DEFAULT_FONT;
		Graphics::Font * font = Assets::Load<Graphics::Font>(DEFAULT_FONT);
		
		// account for padding in 
		const int tileIdX =  w - (font->charW+DEFAULT_TEXT_BORDER) * 3;
		const int tileIdY =  m_box.y - font->charH-DEFAULT_TEXT_BORDER*2- 1;
		
		if ( m_tileIdBox )
			delete m_tileIdBox;
		m_tileIdBox = new UI::TextButton("000", tileIdX, tileIdY, 3, 1, DEFAULT_FONT );

		m_tileIdBox->onUpdate();
	}

	void TilePicker::onDraw()
	{

		Engine::DrawRect( EDITOR_COLOR, m_box, true);		

		if(!m_tileset) return;
		const Rect & worldCursor = 
		{
			//-1 +1 wrap around tile
			m_cursor.x * m_scale + m_box.x - 1, 
			m_cursor.y * m_scale + m_box.y - 1, 
			m_cursor.w * m_scale + 2, 
			m_cursor.h * m_scale + 2 
		};

		const Rect & worldBorder = 
		{
			//-1 +1 wrap around tile
			m_box.x - 1, 
			m_box.y - 1, 
			m_box.w + 2, 
			m_box.h + 2 
		};

		
		const Rect& src = { 0, 0, m_tileset->w, m_tileset->h };
		Engine::DrawTexture( m_tileset->texture, src, m_box);

		Engine::DrawRect(CURSOR_COLOR, worldCursor, false);
		Engine::DrawRect(BORDER_COLOR, worldBorder, false);

		m_tileIdBox->onDraw();

	}

	bool TilePicker::handleKey(Key key, ButtonState state)
	{
		if( state == BUTTON_RELEASE) 
			return false;
		
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
			default:
				return false;

		}
		return true;

	}
} // namespace UI