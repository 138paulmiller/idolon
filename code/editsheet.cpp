#include "graphics.h"
#include "engine.h"
#include "assets.h"
#include "editsheet.h"
#include <iostream>


using namespace Graphics;



void EditSheet::onEnter()
{
	//if sheet exists load, else create
	//Create an overlay to see potential changes. Committing this changes replaces current ui texture.

	Engine::GetSize(w, h);


	s_text = new Graphics::TextBox(18, 5, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	s_text->font = "default";
	s_text->x = 10;
	s_text->y = 1;        
	s_text->reload();

	s_text2 = new Graphics::TextBox (7, 4, "Hello\nWorld\n!!!!!!!!");
	s_text2->font = "default";
	s_text2->y = 100;
	s_text2->textColor = {255, 255, 0, 0};
	s_text2->filled = true;
	s_text2->fillColor = {255, 0, 255, 0};
	s_text2->reload();
	printf("Entered Sheet Editor\n");

}

void EditSheet::onExit()
{
	delete s_text;
	delete s_text2;
	printf("Exited Sheet Editor\n");

}


void EditSheet::onTick()
{
	Engine::ClearScreen();

	if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
	{
		Engine::GetMousePosition(mx, my);
	}
	s_text->draw();
	s_text2->draw();
	const Color &line = {255, 255, 0, 0 };
	Engine::DrawLine(line, 1, 1, w, h);
}

//
void EditSheet::onKey(Key key, bool isDown)
{

}


//Create buttons to resize?
void EditSheet::setSheet(const std::string& name)
{
	s_sheetname = name;
}