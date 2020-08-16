#pragma once

#include "../engine/api.hpp"

//editor "background" color. 
#define EDITOR_COLOR Palette[17]
#define CURSOR_COLOR Palette[25]
#define BORDER_COLOR WHITE

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
	APP_CODE_SHUTDOWN, 	//shutdown system
	//APP_PAUSE,
};


#define SUPPORT(support) (1u << support)

enum AppSupport
{
	APP_SAVE = 0, 
	APP_UNDO,
	APP_REDO,
};



namespace UI
{
	//////////////////////////////////////////////////////////////////////////////////
	class Widget
	{
	public:
		virtual ~Widget() = default;
		virtual void onUpdate() = 0;
		virtual void onDraw() = 0;
		bool hidden = false;
	};
	
	//////////////////////////////////////////////////////////////////////////////////
	class Button : public Widget
	{
	public:
		Button();
		virtual ~Button() = default;
		virtual void onUpdate();
		virtual void onReset();
		virtual void onClick();
		virtual void onHover();

		void click();
		//was in focus on first click, next clicked missed button
		void leave();
		bool isDown();
		bool isDirty();
		virtual void reset();
		const Rect & rect();
		
		std::function<void()> cbLeave;
		std::function<void()> cbClick;
		std::function<void()> cbHover;
		
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
	
	
	//////////////////////////////////////////////////////////////////////////////////
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
		
		Color textColor;
		
	protected:
		Graphics::TextBox * m_textbox; 
	};
	//////////////////////////////////////////////////////////////////////////////////
	/*
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

	private:
		std::string m_textprev ;

		float m_timer;
		bool m_cursorVisible;
	};

	//////////////////////////////////////////////////////////////////////////////////
	//TODO - text input. button on click goes into edit mode. captures key input then releases on enter or click  

	//////////////////////////////////////////////////////////////////////////////////
	class Toolbar : public Widget
	{
		public:
		Toolbar(App* parent, int x, int y);
		~Toolbar();
		int  add(const std::string & text, std::function<void()> click, bool sticky = true);
		void  remove(int id);
		Button *  get(int id);
		
		void onUpdate() override;
		void onDraw() override;
		
		Color textColor;
		Color hoverColor;
		Color clickColor;
		Color fillColor;
		std::string font;
	private:
		const int border = 1;
		//non-owning
		App* m_parent;
		int m_count;
		int m_xoff;
		int m_x, m_y;
		std::vector<int> m_buttonIds;
		//if hiden changes
		bool m_prevHidden;

	};

	//////////////////////////////////////////////////////////////////////////////////
	/*
		Color Selector
			- Renders the sheet at the top-right of the screen.

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
	
	//////////////////////////////////////////////////////////////////////////////////
	/*
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
		void onDraw();
		
		void reload(const std::string & tileset);
		//get tile/frame relative to given sheet
		Rect selection();
		int selectionIndex();
		int indexOf(const Rect & tile);
		void moveCursor(int dx, int dy);
		void resizeCursor(int w, int h);
		void handleKey(Key key, bool isDown);

		const Graphics::Tileset * tileset() const { return m_tileset; } 

	private:
		const Graphics::Tileset * m_tileset;
		Rect m_cursor;

		//entire box
		Rect m_box;
		const int m_scale;
		int m_aspect;

		static const int m_selectionSizes[2][2];
	};



}

