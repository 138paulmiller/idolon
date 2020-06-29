#pragma once
#include "engine.h"
void renderCheck0(int w, int h, float scale)
{
	float timer = 10;
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
		Engine::DrawTexture(texture, { 0,0,w,h}, { 0,0,w,h});
		Engine::DestroyTexture(texture);
	}
}
