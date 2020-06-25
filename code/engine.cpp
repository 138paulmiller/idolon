#include "core.h"

#include "pool.h"
#include "engine.h"
#include <SDL2/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define WINDOW_TITLE "Ult Boy"
#define WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
#define RENDERER_FLAGS SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED


namespace
{
    static bool s_echo;
    static EchoCallback s_echocb;
    //when this key is hit, input handling enters "echo" mode. forwards all key input
    static SDL_Window* s_window;
    static SDL_Renderer* s_renderer;
    static SDL_Color s_clearColor = { 55, 55, 55, 255 };
    //create layers. blit to layer. priority
    //Create internal texture system. All textures are managed by the engine system
    static SDL_Texture* s_buffer;
    static int s_windowW, s_windowH;
    static float s_windowScale;
    
    struct UserEvent
    {

        bool s_isRunning;

        //left, middle, right, x1, x2
        int mousex, mousey;
        int wheelx, wheely;
        int wheeldx, wheeldy;
        ButtonState mousebuttons[5] = { BUTTON_UP } ;
        ButtonState keymap[255] = { BUTTON_UP } ;
    } s_ue;


    static std::vector<SDL_Texture* > s_textures;
    //TODO threaded input
    static std::mutex s_uemutex;

    static uint32_t s_fpsStartTime, s_nextFrameTime, s_frameStartTime, s_startTime, s_frame = 0;
    static float s_fps = 0;
    static uint32_t s_deltaMs;

}

//Functions
namespace Engine
{

    void Startup(int w, int h, float scale)
    {
        s_echo = false;
        s_windowScale = scale;
        s_ue.s_isRunning = SDL_Init(SDL_INIT_EVERYTHING) == 0;
        s_windowW = (int)( w / s_windowScale);
        s_windowH = (int)( h / s_windowScale);
        
        s_window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_X, WINDOW_Y, s_windowW, s_windowH, WINDOW_FLAGS);
        s_renderer = SDL_CreateRenderer(s_window, -1, RENDERER_FLAGS);
        s_buffer = SDL_CreateTexture(s_renderer,
            SDL_PIXELFORMAT_BGRA8888,
            SDL_TEXTUREACCESS_TARGET, 
            w, h);

        s_ue.s_isRunning == s_ue.s_isRunning && s_window && s_renderer;
        s_frame = 0;
        s_startTime = GetTime();
        s_nextFrameTime = s_startTime + FPS_CAP;
        s_fpsStartTime = s_startTime;
        s_frameStartTime = s_startTime;
    }

    void Shutdown()
    {
        for (int i = 0; i < s_textures.size(); i++)
        {
            if (s_textures[i])SDL_DestroyTexture(s_textures[i]);
        }
        SDL_DestroyTexture(s_buffer);
        SDL_DestroyWindow(s_window);
        SDL_Quit();
    }
    void SetEcho(bool on)
    {
        s_echo = on;
    }
    //only use in edit/debug mode
    void SetEchoHandler(EchoCallback cb)
    {
        s_echocb = cb;
    }
    void PollEvents()
    {
        SDL_Event event;

        for(int i = 0; i < 5; i++)
        {
            if (s_ue.mousebuttons[i] == BUTTON_DOWN)
                s_ue.mousebuttons[i] = BUTTON_HOLD;
        }
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYUP:
            {
                unsigned char sym = event.key.keysym.sym;
                if(sym < 255) 
                    s_ue.keymap[sym] = BUTTON_UP;
            }
                break;
            case SDL_KEYDOWN:
            {
                unsigned char sym = event.key.keysym.sym;
                if(sym < 255)
                {

                    if( s_echo )
                    {
                        s_echocb(Key(sym));
                    }
                    ButtonState & state = s_ue.keymap[sym];
                    if (state == BUTTON_DOWN || state == BUTTON_HOLD)
                    {
                        if(Engine::GetTimeDeltaMs() > 10)
                            state = BUTTON_HOLD;
                        else
                            state = BUTTON_UP;
                    }
                    else
                        state = BUTTON_DOWN;
                }
            }
            break;
            case SDL_MOUSEWHEEL:
                s_ue.wheeldx = s_ue.wheelx - event.wheel.x;
                s_ue.wheeldy = s_ue.wheely - event.wheel.y;
                s_ue.wheelx = event.wheel.x;
                s_ue.wheely = event.wheel.y;
                break;
            case SDL_MOUSEMOTION:
                s_ue.mousex = (int)(event.motion.x * s_windowScale);
                s_ue.mousey = (int)(event.motion.y * s_windowScale);
                break;

            case SDL_MOUSEBUTTONDOWN:
                s_ue.mousebuttons[event.button.button-1] = BUTTON_DOWN;
                break;
            case SDL_MOUSEBUTTONUP:
            
                s_ue.mousebuttons[event.button.button-1] = BUTTON_UP;
                break;
            case SDL_QUIT:
                s_ue.s_isRunning = false;
                break;
            default:
                break;
            }
        }
    }

    void Render()
    {
        //clear screen
        SDL_SetRenderTarget(s_renderer, 0);
        SDL_SetRenderDrawColor(s_renderer, s_clearColor.r, s_clearColor.g, s_clearColor.b, s_clearColor.a);
        SDL_RenderClear(s_renderer);
        //deqeue commands until timer is up
        //render all ui objects
        
        SDL_Rect drect = { 0,0, s_windowW,s_windowH };

        SDL_RenderCopy( s_renderer, s_buffer, NULL, &drect );
        SDL_RenderPresent(s_renderer);
    }

    void UpdateTime()
    {
        int waitMS = (int)((1.f / FPS_CAP * 1000.f) -  (GetTime() - s_frameStartTime));
        waitMS = waitMS > 0 ? waitMS : 0;
        SDL_Delay(waitMS);
        if (++s_frame > FPS_SAMPLE_COUNT)
        {
            const uint32_t time = GetTime();
            s_fps = (FPS_SAMPLE_COUNT/(( time - s_fpsStartTime) / 1000.0f));
            s_fpsStartTime = time;
            s_frame = 0;
        }       
        s_deltaMs = (GetTime() - s_frameStartTime);
    }

    bool Run()
    {
        s_frameStartTime  = GetTime();
        PollEvents();
        if (!s_ue.s_isRunning) 
            return false;
        Render(); 
        UpdateTime();
        return true;
    }  
    void Resize(int w, int h)
    {
        int sw = (int)(w / s_windowScale);
        int sh = (int)(h / s_windowScale);
        SDL_SetWindowSize(s_window, sw, sh);
        SDL_GetWindowSize(s_window, &s_windowW, &s_windowH);
        if (s_buffer)
        {
            SDL_DestroyTexture(s_buffer);
        }
        s_buffer = SDL_CreateTexture(s_renderer,
            SDL_PIXELFORMAT_BGRA8888,
            SDL_TEXTUREACCESS_TARGET, 
            w, h);
        
    }
    
    int CreateTexture(int width, int height)
    {
        //find next slot
        int i;
        for (i = 0; i < s_textures.size(); i++)
        {
            if (s_textures[i] == 0)
                break;
        }
        if (i >= s_textures.size()) {
            s_textures.push_back(0);
        }
        s_textures[i] = SDL_CreateTexture(s_renderer,
            SDL_PIXELFORMAT_BGRA8888,
            SDL_TEXTUREACCESS_STREAMING, 
            width, height);
        return i;
    }
    void DestroyTexture(int textureId)
    {  
        ASSERT(textureId >= 0 && textureId < s_textures.size() && s_textures[textureId], "Engine: Texture does not exist");
        SDL_Texture* texture = s_textures[textureId];
        SDL_DestroyTexture(texture);
        s_textures[textureId] = 0;
    }

    Color * LockTexture(int textureId, const Rect & region)
    {
        ASSERT(textureId >= 0 && textureId < s_textures.size() && s_textures[textureId], 
            "Engine: Texture does not exist");
        SDL_Texture* texture = s_textures[textureId];

        uint32_t format;
        int access, buffw, buffh;
        SDL_QueryTexture(texture, &format, &access, &buffw, &buffh);
        ASSERT(region.x+region.w <= buffw && region.y+region.h <= buffh && region.x >= 0 && region.y >= 0 && region.w >= 0 && region.h >= 0, "LockTexture: Invalid rect");
        const SDL_Rect & rect = { region.x,region.y,region.w,region.h };
        //internal 
        int pitch;
        Color* texpixels = 0;
        SDL_LockTexture(texture, &rect, (void**)&texpixels, &pitch);
        return texpixels;
    }   
    void UnlockTexture(int textureId)
    {
        ASSERT(textureId >= 0 && textureId < s_textures.size() && s_textures[textureId], 
            "Engine: Texture does not exist");
        SDL_Texture* texture = s_textures[textureId];
        SDL_UnlockTexture(texture);
    }
    Color * LoadTexture(const std::string path, int &w, int &h)
    {
        int bpp = 4; //force 4 bpp
        unsigned char *data = stbi_load(path.c_str(), &w, &h, &bpp, bpp);
        Color * pixels = new Color[w*h];
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                Color* c = &pixels[y * w + x];
                c->r = data[y * w * 4 + x + 0];
                c->g = data[y * w * 4 + x + 1];
                c->b = data[y * w * 4 + x + 2];
                c->a = data[y * w * 4 + x + 3];
            }
        }
        stbi_image_free(data);
        return pixels;
    }
    
    void DrawTexture(int textureId, const Rect & src, const Rect & dest)
    {
        ASSERT(textureId >= 0 && textureId < s_textures.size() && s_textures[textureId], 
            "Engine: Texture does not exist");
        SDL_Texture* texture = s_textures[textureId];
        const SDL_Rect & srcrect = { src.x, src.y, src.w, src.h };
        const SDL_Rect& destrect = { dest.x, dest.y,dest.w,dest.h};
        SDL_SetRenderTarget(s_renderer, s_buffer);
	    SDL_RenderClear(s_renderer);
        SDL_RenderCopy( s_renderer, texture, &srcrect, &destrect );
    }

    void DrawLine(const Color& color, int x1, int y1, int x2, int y2)
    {
        SDL_SetRenderTarget(s_renderer, s_buffer);
        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(s_renderer, x1, y1, x2, y2);
    }

    void DrawRect(const Color& color, const Rect & rect, bool filled)
    {
        SDL_SetRenderTarget(s_renderer, s_buffer);
        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
        SDL_Rect sdlrect = { rect.x, rect.y,rect.w,rect.h };
        if (filled)
            SDL_RenderFillRect(s_renderer,&sdlrect);
        else
            SDL_RenderDrawRect(s_renderer,&sdlrect);
    }

    // ----------- Getters . Should Inline these with externs! ---------------------------
    
    ButtonState GetKeyState(Key key)
    {
        return s_ue.keymap[(int)key];
    }

    ButtonState GetMouseButtonState(MouseButton button)
    {
        return s_ue.mousebuttons[button];
    }

    void GetMousePosition(int& x, int& y)
    {
        x = s_ue.mousex;
        y = s_ue.mousey;
    }

    void GetMouseWheel(int & x, int & y, int & dx, int & dy)
    {
        x = s_ue.wheelx;
        y = s_ue.wheely;
        dx = s_ue.wheeldx;
        dy = s_ue.wheeldy;
    }
    
    void GetSize(int & w, int & h)
    {
        w = (int)(s_windowW * s_windowScale);
        h = (int)(s_windowH * s_windowScale);
    }

    uint32_t GetTime()
    {
        return SDL_GetTicks() - s_startTime;
    }
    uint32_t GetTimeDeltaMs()
    {
        return s_deltaMs;
    }

    float GetFPS()
    {
        return s_fps;
    }

}