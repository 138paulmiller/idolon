#pragma once
#include "core.h"

//Classes that can be used by UI "pages"
namespace Graphics 
{
	class Font;
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
