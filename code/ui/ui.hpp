#pragma once

#include "../engine/api.hpp"


//Classes that can be used by UI 
class App;
namespace Graphics 
{
	class Font;
	class Tileset;
	class Sprite;
	class TextBox;
}

enum AppCode
{
	APP_CODE_CONTINUE = 0, 
	APP_CODE_EXIT, 		//exit app
	APP_CODE_SHUTDOWN, 	//shutdown systemicons.
	//APP_PAUSE,
};


#define SUPPORT(support) (1u << support)

enum AppSupport
{
	APP_SAVE = 0, 
	APP_UNDO,
	APP_REDO,
	APP_SUPPORT_COUNT
};



namespace UI
{

	/*-------------------------------------------------------------------
	*/
	class Widget
	{
	public:
		virtual ~Widget() = default;
		virtual void onUpdate() = 0;
		virtual void onDraw() = 0;
		bool hidden = false;
	};
	

	/*-------------------------------------------------------------------
	*/	
	class Button : public Widget
	{
	public:
		Button();
		virtual ~Button() = default;
		virtual void onUpdate() override;
		virtual void onDraw() override;

		virtual void onReset();
		virtual void onClick();
		virtual void onHover();

		void click();
		//was in focus on first click, next clicked missed button
		void leave();
		bool isDown() const;
		bool isDirty() const;
		virtual void reset();
		const Rect & rect() const;
		void setRect( int x, int y, int w, int h );

		std::function<void()> cbLeave;
		std::function<void()> cbClick;
		std::function<void()> cbHover;
		std::function<void()> cbHoverOff;
		//User settings
		Color hoverColor;
		Color clickColor;
		Color fillColor;
		bool sticky = true;
		
	protected: 
		bool m_prevclick;
		Rect m_rect;
		//internal settings
		Color m_color;
	private: 
		bool m_isDirty;
		bool m_isHover;
		bool m_isDown;
	};
	

	/*-------------------------------------------------------------------
	*/
	class TextButton : public Button
	{
	public:
		TextButton(const std::string & text, int x, int y, int tw, int th, const std::string & font);
		~TextButton();
		
		virtual void onUpdate() override ;
		virtual void onDraw() override;
		//if font is changed reload
		void setFont(const std::string & font) ;
		void setText(const std::string & text) ;
		void setEscape( bool isEscaped ) ;
		
		Color textColor;

	protected:
		Graphics::TextBox * m_textbox; 
	};
	/*-------------------------------------------------------------------
		Input
	*/

	class TextInput : public TextButton
	{
	public:
		TextInput(const std::string & text, int x, int y, int tw, int th, const std::string & font);
		~TextInput();

		std::string text ;
		std::function<void()> cbAccept ;
		//draw cursor
		void onDraw() override;
		void setText(const std::string & text);

	private:
		std::string m_textprev ;

		float m_timer;
		bool m_cursorVisible;
	};

	/*-------------------------------------------------------------------
	*/

	class ScrollBar : public Widget
	{
	public:
		ScrollBar(App* parent, int x, int y, int len, bool isHorizontal = false );
		~ScrollBar ();
		void onDraw() override;
		void onUpdate() override;
		
		void hide( bool isHidden );
		void setRange( int step, int total );
		void scrollBy(float d);
		  
		std::function<void( int )> cbScroll;

	private:
		bool m_isHoriz;
		int m_step, m_total, m_len;
		//percentage of movement across bars
		int m_scrollDir;
		float m_scroll;
		float m_scrollSpeed = 0.15;
		int m_upBtn, m_downBtn, m_barBtn;
		App *m_parent;
	};

	/*-------------------------------------------------------------------
	*/
	class TextScrollArea : public Widget
	{
	public:
		TextScrollArea(App * parent,int x, int y, int w, int h, const std::string & fontname);
		~TextScrollArea();
		void onUpdate() override;
		void onDraw() override;
		bool handleKey(Key key, ButtonState state);

		void setText(const std::string & text);
		const std::string & getText();
		void scrollPageBy(int dx, int dy);

		void hide(bool isHidden);
		void resetCursor();
	private:

		void scrollTextBy(int dx, int dy);
		void updateTextOffset();

		Graphics::TextBox * m_textBox;
		Graphics::TextBox *m_cursor;
		UI::ScrollBar *m_scrollBar  ;

		int m_cursorX, m_cursorY;
		uint m_cursorPos;
		
		//chars per line width, height
		int m_textW;
		int m_textH;
		//size of char in pixels
		int m_charW;
		int m_charH;
		int m_lines;
		//flicker timer 
		float m_timer;
		bool m_hidden;
		const int m_scrollBarWidth = 12;

	};

	/*-------------------------------------------------------------------
	*/
	class Toolbar : public Widget
	{
		public:
		Toolbar(App* parent, int x, int y);
		~Toolbar();
		void onUpdate() override;
		void onDraw() override;
		
		int  add(const std::string & text, std::function<void()> click, bool sticky = true, bool isEscaped = true);
		void  remove(int id);
		Button *  get(int id);
		
		Color textColor;
		Color hoverColor;
		Color clickColor;
		Color fillColor;
		std::string font;
		//add buttons from left to right. 
		bool leftAlign;

		//add buttons from top to bottom. 
		bool verticalAlign;

	private:
		int m_x, m_y;
		const int border = 1;
		//non-owning
		App* m_parent;
		int m_count;
		//x or y offset
		int m_off;
		std::vector<int> m_buttonIds;
		//if hiden changes
		bool m_prevHidden;

	};

	class ComboBox : public Widget
	{
	public:
		ComboBox( App *parent, int x, int y, int tw, int th);
		~ComboBox();
		void onUpdate() override;
		void onDraw() override;
		
		void select( int index );
		void add(const std::string & text );
		void  remove( const std::string & text  );
		void open();
		void close();
		std::function<void(const std::string &)> cbSelect;

	private:
		
		void updateInput();
		
		int m_x, m_y, m_tw, m_th;
		App *m_parent;
		bool m_isOpen, m_requiresClear;
;
		int m_selection;
		//selected option. 
		int m_textInputId;
		int m_openButtonId;
		//selections
		std::vector<int> m_selectionIds;
		std::vector<std::string> m_selections;
	};

	/*-------------------------------------------------------------------
		Color Selector
			- Renders the sheet at the top-right of the screen
	*/
	class ColorPicker : public Widget
	{
	public:
		ColorPicker(int x, int y);
		~ColorPicker();

		void onUpdate() override;
		void onDraw() override;

		Color color();
	private:
		Graphics::Tileset * m_tileset;
		int m_colorSize;
		Rect m_cursor;
		Rect m_src, m_dest, m_border;
		Color m_color;
		Color m_cursorColor;
		Color m_borderColor;
	};
	
	/*-------------------------------------------------------------------
		Tileset Tile/Frame picker
		- Renders the sheet at the bottom of the screen.
	*/
	class TilePicker : public Widget
	{
	public:
		TilePicker();
		~TilePicker();

		const Rect & rect();
		void onUpdate() override;
		void onDraw() override;
		
		void reload(const Graphics::Tileset * tileset);
		//get tile/frame relative to given sheet
		Rect selection();
		int selectionIndex();
		void moveCursor(int dx, int dy);
		void resizeCursor(int w, int h);
		bool handleKey(Key key, ButtonState state) ;

		const Graphics::Tileset * tileset() const { return m_tileset; } 

	private:
		//create hide tab as well as tileset source
		UI::TextButton * m_tileIdBox;

		const Graphics::Tileset * m_tileset;
		Rect m_cursor;

		//entire box
		Rect m_box;
		const int m_scale;
		int m_aspect;
		bool m_focus;

		static const int m_selectionSizes[2][2];
	};



}

