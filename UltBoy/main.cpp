#include "pch.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "editor.h"


#define GRID_W 200
#define GRID_H 150
#define WINDOW_SCALE 1.0/5
#define SIM_RATEMS 10


void renderCheck0(int w, int h, float scale)
{
	Engine::Startup( w, h, scale);
	float timer = 10;
	int texture = 0;

	while (Engine::Run())
	{
		timer -= Engine::GetTimeDeltaMs()/1000.0;
		//every 1 seconds
		if (timer <= 0) 
		{
			printf("FPS:%.2f\n", Engine::GetFPS());
			timer = 10;
		}

		//perf test. create destory
		int texture = Engine::CreateTexture(w, h);
		const int len = w * h;
		Color* pixels = Engine::LockTexture(texture, { 0,0,w,h });

		for(int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			const int i = y * w + x;

			Color& pixel = pixels[i];
			pixel.r = ((float)x) / w * 255;
			pixel.g = ((float)y) / h*255;
			pixel.b = 0;
			pixel.a = 255;
		}
		Engine::UnlockTexture(texture);
		Engine::UseTexture(texture);
		Engine::Draw({ 0,0,w,h}, { 0,0,w,h});
		Engine::DestroyTexture(texture);
	}
	Engine::Shutdown();
}

int main(int argc, char** argv)
{ 

	Engine::Startup( GRID_W, GRID_H, WINDOW_SCALE);
	Editor::RunSheetView("example");
	Engine::Shutdown();

	return 0;
}