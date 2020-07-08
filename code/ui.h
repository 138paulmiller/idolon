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

namespace UI
{
	class Element
	{
	public:
		virtual ~Element() = default;
		virtual void onUpdate() = 0;
		virtual void onDraw() = 0;
	};

	class Button : public Element
	{
	public:
		Button();
		virtual ~Button() = default;
		const Rect & rect();

		virtual void onClick() = 0;
		virtual void onHover() = 0;

		std::function<void()> cbClick;
		std::function<void()> cbHover;
	protected: 
		Rect m_rect;
	};

	//TODO Create universal toolbar that runs when editor is running. Used to navigate the various editor pages. return back to terminal
	//This toolbar broadcasts events that each UI can regoister to
	class Widget
	{
	public:
		virtual ~Widget();
		virtual void onEnter() = 0;
		virtual void onExit() = 0;
		virtual void onTick() = 0;
		virtual void onKey(Key key, bool isDown) = 0;
	
		void update();
		void draw();

	protected:
		void clear();


		int addElement(Element * element);
		int addButton(Button * button);
	
		Button * getButton(int idx);
		Element * getElement(int idx);

		void removeButton(int idx);
		void removeElement(int idx);
	
	private:

		std::vector<Element*> m_elements;
		std::vector<Button*> m_buttons;
	};


	class TextButton : public Button
	{
	public:
		TextButton(const std::string & text, int x, int y, int tw, int th);
		~TextButton();
		void onUpdate() ;
		void onDraw() ;
		void onHover() ;
		void onClick() ;
		Color hoverTextColor;
		Color hoverColor;
		Color textColor;
		Color color;
	private: 
		bool m_isDirty;
		Graphics::TextBox * m_textbox; 
	};




	/*
		Create Color Selector
			- Renders the sheet at the top-right of the screen.

	*/

	class ColorPicker : public Element
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

	/*
		Sheet Tile/Frame picker
		- Renders the sheet at the bottom of the screen.
	*/
	class SheetPicker : public Element
	{
	public:
		SheetPicker(const Graphics::Sheet * sheet);
		~SheetPicker();

		void onUpdate();
		void onDraw();

		//get tile/frame relative to given sheet
		Rect selection();
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

