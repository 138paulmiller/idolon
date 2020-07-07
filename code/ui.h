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


//TODO Create universal toolbar that runs when editor is running. Used to navigate the various editor pages. return back to terminal
//This toolbar broadcasts events that each UI can regoister to
class UI 
{
public:
	virtual ~UI() = default;
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	virtual void onTick() = 0;
	virtual void onKey(Key key, bool isDown) = 0;
};


/*
	Create Color Selector
*/

class ColorPicker
{

};

/*
	Sheet Tile/Frame picker
	- Renders the sheet at the bottom of the screen.
*/
class SheetPicker
{
public:
	SheetPicker(const Graphics::Sheet * sheet);
	~SheetPicker();
	//get tile relative to given sheet
	Rect selection();
	//updates cursor
	void update();
	void draw();
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
