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

	//name of sheet to edit
	int RunSheetView(const std::string& name)
	{
		//if sheet exists load, else create
		//Create an overlay to see potential changes. Committing this changes replaces current ui texture.
		Engine::SetKeyEcho(true);
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
			text.draw();
	        text2.draw();
	        const Color &line = {255, 255, 0, 0 };
			Engine::DrawLine(line, 1, 1, w, h);
		}
		Assets::Unload(name);
		Engine::SetKeyEcho(false);

		return 1;
	}
}