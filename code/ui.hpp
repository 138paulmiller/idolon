#pragma once
#include "core.hpp"


//Classes that can be used by UI "pages"
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
		bool isDown();
		bool isDirty();
		virtual void reset();
		const Rect & rect();
		
		std::function<void()> cbClick;
		std::function<void()> cbHover;
		
		//User settings
		Color hoverColor;
		Color clickColor;
		Color fillColor;
		bool sticky = true;
		
	protected: 
		Rect m_rect;
		//internal settings
		Color m_color;
	private: 
		bool m_isDirty;
		bool m_isHover;
		bool m_isDown;
	};
	
	//////////////////////////////////////////////////////////////////////////////////

	class App
	{
	public:
		App();
		virtual ~App();
		virtual void onEnter() = 0;
		virtual void onExit() = 0;
		virtual void onTick() = 0;
		virtual void onKey(Key key, bool isDown) = 0;
							
	
		void signal(AppCode code);
		AppCode status();

		void update();
		void draw();

		void clear();

		int addWidget(Widget * widget);
		int addButton(Button * button);
	
		Button * getButton(int idx);
		Widget * getWidget(int idx);

		int getButtonCount();
		int getWidgetCount();

		void removeButton(int idx);
		void removeWidget(int idx);
	
	private:
		AppCode m_status;
		std::vector<Widget*> m_widgets;
		std::vector<Button*> m_buttons;
	};

	
	//////////////////////////////////////////////////////////////////////////////////
	class TextButton : public Button
	{
	public:
		TextButton(const std::string & text, int x, int y, int tw, int th);
		~TextButton();
		void onUpdate() override ;
		void onDraw() override;
		//if font is changed reload
		void setFont(const std::string & font) ;
		void setText(const std::string & text) ;
		
		Color textColor;
		
	protected:
		Graphics::TextBox * m_textbox; 
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

	};

	//////////////////////////////////////////////////////////////////////////////////
	/*
		Color Selector
			- Renders the sheet at the top-right of the screen.

	*/

	class TextInput : public TextButton
	{
	public:
		TextInput(const std::string & text, int x, int y, int tw, int th);
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

