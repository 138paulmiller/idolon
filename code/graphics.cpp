#include "core.h"
#include "graphics.h"
#include "assets.h"
#include "engine.h"

#include <sstream>

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
    
    ///////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::Sprite(int w, int h, const std::string& name)
        :Asset(name), sheetcache(0), iframe(0), timer(0), animation("default"), rect({0,0,w,h})
    {
    }
    void Sprite::reload()
    {
        sheetcache = Assets::Load<Sheet>(sheet);
    }

    void Sprite::draw()
    {
        timer += Engine::GetTimeDeltaMs();
        const std::vector<Frame>& frames = animframes[animation];
        
        if (timer > frames[iframe].duration)
        {
            timer = 0;
            iframe++;
            iframe %= frames.size();
        }
        Engine::DrawTexture(sheetcache->texture, frames[iframe].region, rect);
    }
    
    ///////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::string& name, int w, int h, int charW, int charH, char start)
         :Sheet(name, w, h), 
         charW(charW), charH(charH), 
         start(start)
    { 
    }    

    void Font::blit(int destTexture, const std::string & text, const Rect & dest)
    {

        int c, sx,sy;
        const int srcW = w / charW;  
        const int destW = dest.w / charW;     
        const int destH = dest.h / charH;     
        int scrolly = 0;
        int dx = 0, dy = 0;
        for(int i = 0; i < text.size(); i++)
        {
            int s =  text[i];
            if(s == '\n')
            {
                i++;
                if(i== text.size()) break;
                scrolly++;
                s =  text[i];
                dx = 0;
                dy++;
            }
            if(dx == destW)
            {
                dx = 0;
                dy++;
            }
            c =  s - start;
            sx = (c % srcW);
            sy = (c / srcW) ;            
            Rect src = { 
                sx * charW, sy * charH, 
                charW, charH 
            } ;
            Rect pos = { 
                dest.x + dx * charW, 
                dest.y + dy * charH, 
                charW, charH 
            };   
            if(dy > destH) 
                break;
            Engine::Blit(texture, destTexture, src, pos);
            dx++;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    
    TextBox::TextBox(int w, int h, const std::string & text)
        :text(text),
        font("default"),
        texture (0),
        x(0), y(0), 
        w(0), h(0),
        tw(w), th(h),
        scrolly(0),filled(false),
        textColor({255,255,255,255}),
        fillColor({255,0,0,0}),
        visible(true)
    {
    }
    TextBox::~TextBox()
    {
        Engine::DestroyTexture(texture);
    }

    void TextBox::draw()
    {
        if (!visible) return;
        if(!fontcache) return;
        if(filled) Engine::DrawRect(fillColor, {x,y,w,h}, true);
        Engine::DrawTexture(texture, {0,0,w,h}, {x,y,w,h});
    }

    void TextBox::refresh()
    {
        Engine::ClearTexture(texture, fillColor);
        fontcache->blit(texture, text, {0,0,w,h});
    }
    void TextBox::reload()
    {
        fontcache = Assets::Load<Font>(font);
        if(!fontcache) return;
        //only do if texture w/h changes
        if(texture) Engine::DestroyTexture(texture);
        w = tw * fontcache->charW;
        h = th * fontcache->charH;
         
        texture = Engine::CreateTexture(w, h, true);
        Engine::MultiplyTexture(texture, textColor);
        refresh();
    }
}