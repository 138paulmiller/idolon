#include "graphics.h"
#include "engine.h"
#include "assets.h"
#include <iostream>

namespace Editor
{
	using namespace Graphics;

	void Startup()
	{}

	void Shutdown()
	{}
	//Create buttons to resize?
	int EditSheet(const std::string& name)
	{
		Engine::ClearScreen();
		//if sheet exists load, else create
		//Create an overlay to see potential changes. Committing this changes replaces current ui texture.
		int mx, my;
		int w, h;
		Engine::GetSize(w, h);


		Graphics::TextBox text(18, 5, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
			text.font = "default";
			text.x = 10;
			text.y = 1;        
			text.reload();

		Graphics::TextBox text2(7, 4, "Hello\nWorld\n!!!!!!!!");
			text2.font = "default";
			text2.y = 100;
			text2.textColor = {255, 255, 0, 0};
			text2.filled = true;
			text2.fillColor = {255, 0, 255, 0};
			text2.reload();

		while (Engine::Run())
		{
			if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
			{
				Engine::GetMousePosition(mx, my);
			}
			if (Engine::GetKeyState(KEY_ESCAPE) == BUTTON_DOWN)
			{
				break; //exit loop
			}
			text.draw();
	        text2.draw();
	        const Color &line = {255, 255, 0, 0 };
			Engine::DrawLine(line, 1, 1, w, h);
		}
		
		Engine::ClearScreen();

		return 1;
	}

	int EditFont(const std::string& name)
	{
		return 1;
	}
}