#include "pch.h"
#include "graphics.h"
#include "assets.h"
#include "Engine.h"


namespace Graphics
{
    

    Sheet::Sheet(const std::string& name, int w, int h)
        :Asset(name), w(w), h(h),
        pixels(new Color[(int)(w * h)])
    {
        memset(pixels, 0, w * h * sizeof(Color));

        texture = Engine::CreateTexture(w, h); 

    }
    Sheet::~Sheet()
    {
        delete[]pixels;
    }

    void Sheet::update()
    {
        //load pixels
        Color * data = Engine::LockTexture(texture, { 0,0,w, h});

        memcpy(data, pixels, w * h * sizeof(Color));
        Engine::UnlockTexture(texture);

    }
    void Sheet::use()
    {
        Engine::UseTexture(texture);
    }

    Sprite::Sprite(const std::string& name)
        :Asset(name), sheetcache(0), iframe(0), timer(0), animation("default")
    {
        rect = { 0,0,0,0 };
    }
    void Sprite::update()
    {
        if (sheetcache)
            sheetcache = Assets::Load<Sheet>(name);
    }

    void Sprite::draw()
    {
        timer += Engine::GetTimeDeltaMs();
        const std::vector<Frame>& frames = animframes[animation];
        
        Frame frame = frames[iframe];
        if (timer > frame.duration)
        {
            timer = 0;
            iframe++;
            iframe %= frames.size();
            frame = frames[iframe];
        }
        sheetcache->use();
        Engine::Draw(frame.region, rect);
    }

}