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
		//Create an overlay to see potential changes. Committing this changes replaces current ui texture.
		Engine::SetEcho(true);
		int mx, my;
		int w, h;
		Engine::GetSize(w, h);
		Rect dest = { 0,0,w,h };
		Sheet* sheet = Assets::Load<Sheet>(name);
		if (!sheet)
		{
			//add a create new sheet button
			sheet = new Sheet(name, w, h);
		}

		Graphics::Text text(128, 70, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
        text.crop = false;
        text.font = "default";
        text.reload();

		Graphics::Text text2(35, 70, "Hello\nWorld");
        text2.y = 100;
        text2.crop = false;
        text2.font = "default";
        text2.reload();


		Rect src = { 0,0,sheet->w,sheet->h };
		float timer = 0;
		while (Engine::Run())
		{
			timer += Engine::GetTimeDeltaMs()/1000.0f;
			//every 1 seconds
			if (timer > 1.f) 
			{
				printf("FPS:%.2f\n", Engine::GetFPS());
				timer = 0.f;
			}
			if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
			{
				Engine::GetMousePosition(mx, my);
				Color color;
				color.r = 255;
				color.g = 255;
				color.b = 255;
				color.a = 255;
				sheet->pixels[my * sheet->w + mx] = color;
				sheet->update({mx, my, 1, 1 });
			}
		//		Engine::DrawTexture(sheet->texture, src, dest);
	        text.draw();
	        text2.draw();

		}
		Assets::Save(sheet, name);
		Assets::Unload(name);
		Engine::SetEcho(false);

		return 1;
	}
}