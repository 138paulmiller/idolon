#include "core.h"
#include "graphics.h"
#include "assets.h"
#include "engine.h"

namespace Graphics
{
    

    Sheet::Sheet(const std::string& name, int w, int h)
        :Asset(name), w(w), h(h),
        pixels(new Color[(int)(w * h)]),
        texture(Engine::CreateTexture(w, h))
    {
        memset(pixels, 0, w * h * sizeof(Color));
 

    }
    Sheet::~Sheet()
    {
        delete[]pixels;
        Engine::DestroyTexture(texture);
    }

    void Sheet::update(const Rect & rect )
    {
        ASSERT(rect.x >= 0 && rect.w+rect.x <= w 
            && rect.y >= 0 && rect.h+rect.y <= h,
            "Engine: Invalid rect" );

        if( rect.w == 0 || rect.h == 0 )
        {
            Color * data = Engine::LockTexture( texture, { 0,0,w,h } );
            memcpy(data, pixels, w * h * sizeof(Color));
        }
        else
        {
            Color * data = Engine::LockTexture( texture, rect );
            for(int y = 0; y < rect.h; y++)
                for(int x = 0; x < rect.w; x++)
                    data[y * w  + x] = pixels[(rect.y+y) * w  + rect.x + x];
        }   

        Engine::UnlockTexture(texture);

    }
    Sprite::Sprite(const std::string& name)
        :Asset(name), sheetcache(0), iframe(0), timer(0), animation("default")
    {
        rect = { 0,0,0,0 };
    }
    void Sprite::reload()
    {
        if (!sheetcache)
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
        Engine::DrawTexture(sheetcache->texture, frame.region, rect);
    }

}