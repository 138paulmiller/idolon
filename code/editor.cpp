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
		Rect src =  { 0,0,w,h }, dest = { 0,0,w,h };
		Sheet* sheet = Assets::Load<Sheet>(name);
		if (!sheet)
		{
			//add a create new sheet button
			sheet = new Sheet(name, w, h);
		}
 		//Color * pixels = Sim::GetPixels();
		float timer = 0;
		while (Engine::Run())
		{
			timer += Engine::GetTimeDeltaMs()/1000.0;
			//every 1 seconds
			if (timer > 1) 
			{
				printf("FPS:%.2f\n", Engine::GetFPS());
				timer = 0;
			}
			if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
			{
				Engine::GetMousePosition(mx, my);
				Color color;
				color.r = 255;
				color.g = 255;
				color.b = 255;
				color.a = 255;
				sheet->pixels[my * w + mx] = color;
				if(mx > 0) mx--;
				if(my > 0) my--;
				
				sheet->update({mx, my, 2, 2 });
			}
			Engine::DrawTexture(sheet->texture, src, dest);
		}
		Assets::Save(sheet, name);
		Assets::Unload(name);
		Engine::SetEcho(false);

		return 1;
	}
}