#pragma once
#include "core.h"

//Classes that can be used by UI "pages"
namespace Graphics 
{
	class Font;
	class Sheet;
	class Sprite;
	class TextBox;
}

enum AppCode
{
	APP_CODE_CONTINUE = 0, 
	APP_CODE_EXIT, 		//exit app
	APP_CODE_SHUTDOWN, 	//shutdown system
	// ? 
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
		void onUpdate() ;
		void onDraw() ;
		
		Color textColor;

	private:
		Graphics::TextBox * m_textbox; 
	};
	//////////////////////////////////////////////////////////////////////////////////
	class Toolbar : public Widget
	{
		public:
		Toolbar(App* parent, int x, int y);
		~Toolbar();
		int  add(const std::string & text, std::function<void()> click, bool sticky = true);
		void  remove(int id);
		Button *  get(int id);
		
		void onUpdate();
		void onDraw() ;
		Color textColor;
		Color hoverColor;
		Color clickColor;
		Color fillColor;
	private:
		const int border = 1;
		//non-owning
		App* m_parent;
		int m_x, m_y, m_tw, m_th;
		int m_count;
		int m_xoff;
		std::vector<int> m_buttonIds;

	};


	//////////////////////////////////////////////////////////////////////////////////
	/*
		Create Color Selector
			- Renders the sheet at the top-right of the screen.

	*/

	class ColorPicker : public Widget
	{
	public:
		ColorPicker();
		~ColorPicker();

		void onUpdate();
		void onDraw();

		Color color();
	private:
		Graphics::Sheet * m_sheet;
		int m_colorSize;
		Rect m_cursor;
		Rect m_src, m_dest, m_border;
		Color m_color;
	};
	
	//////////////////////////////////////////////////////////////////////////////////
	/*
		Sheet Tile/Frame picker
		- Renders the sheet at the bottom of the screen.
	*/
	class SheetPicker : public Widget
	{
	public:
		SheetPicker(const Graphics::Sheet * sheet);
		~SheetPicker();

		const Rect & rect();
		void onUpdate();
		void onDraw();
		void setSheet(const Graphics::Sheet * sheet);
		//get tile/frame relative to given sheet
		Rect selection();
		int selectionIndex();
		void moveCursor(int dx, int dy);
	private:
		const Graphics::Sheet * m_sheet;
		Rect m_cursor;
		//draw  src and dest of texture. 
		// draw top half of texture to the left
		Rect m_srcLeft, m_destLeft;
		Rect m_srcRight, m_destRight;
		//entire box
		Rect m_box;
	};



}

