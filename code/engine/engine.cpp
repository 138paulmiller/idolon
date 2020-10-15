

#include "core.hpp"
#include "engine.hpp"
#include <stack>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


#define WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define WINDOW_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
#define RENDERER_FLAGS SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED


#define CHECK_TEXTURE(texId) ASSERT(texId >= 0 && texId < s_textures.size() && s_textures[texId], "Engine::Blit: Texture does not exist");


namespace
{
    struct UserEvent
    {

        bool s_isRunning;

        //left, middle, right, x1, x2
        int mousex, mousey;
        int wheelx, wheely;
        int wheeldx, wheeldy;
        ButtonState mousebuttons[5] = { BUTTON_RELEASE } ;
        ButtonState keymap[512] = { BUTTON_RELEASE } ;
    } s_ue;

 
    bool s_echo; 
    std::stack<  KeyHandler > s_keyhandlers;
    //when this key is hit, input handling enters "echo" mode. forwards all key input
    SDL_Window* s_window;
    SDL_Renderer* s_renderer;
    Color s_clearColor = BLACK;
    //create layers. blit to layer. priority
    //Create internal texture system. All textures are managed by the engine system
    int s_target; //used to upsample to display to acheive pixelated effect
    int s_bound; //texture currently bound render target
    int s_windowW, s_windowH;
    float s_windowScale;
    int s_alignX = 1, s_alignY = 1;
    

    std::vector<SDL_Texture* > s_textures;
    uint32_t s_fpsStartTime, s_nextFrameTime, s_frameStartTime, s_startTime, s_frame = 0;
    float s_fps = 0;
    uint32_t s_deltaMs;

} // namespace

//Functions
namespace Engine
{

    void Startup(const char * title, int w, int h, float scale)
    {
        s_echo = false;
        s_windowScale = scale;
        s_ue.s_isRunning = SDL_Init(SDL_INIT_EVERYTHING) == 0;
        
        
        s_windowW = (int)( w * s_windowScale);
        s_windowH = (int)( h * s_windowScale);

        s_window = SDL_CreateWindow(title, WINDOW_X, WINDOW_Y, s_windowW, s_windowH, WINDOW_FLAGS);
        s_renderer = SDL_CreateRenderer(s_window, -1, RENDERER_FLAGS);
        s_target = CreateTexture( w, h, TEXTURE_TARGET);

        s_ue.s_isRunning = (s_ue.s_isRunning && (s_window && s_renderer));
        s_frame = 0;
        s_startTime = GetTime();
        s_nextFrameTime = s_startTime + FPS_CAP;
        s_fpsStartTime = s_startTime;
        s_frameStartTime = s_startTime;
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); 
    }

    void Shutdown()
    {
		LOG("Engine: Shutting down...\n");
        for (int i = 0; i < s_textures.size(); i++)
        {
            if (s_textures[i])SDL_DestroyTexture(s_textures[i]);
        }
        SDL_DestroyWindow(s_window);
        SDL_Quit();
    }
    void SetKeyEcho(bool on)
    {
        s_echo = on;
        if (!on)
            s_echo = 0;
    }
    void PushKeyHandler(KeyHandler cb)
    {
        s_keyhandlers.push( cb );

    }

    void PopKeyHandler()
    {
        s_keyhandlers.pop( );
    }
    void AlignMouse(int x, int y)
    {
        s_alignX = x;
        s_alignY = y;
    }

    //forward 
    Key GetKeyFromKeyCode(SDL_Keycode code, bool cap);
    
    void PollEvents()
    {
        SDL_Event event;

        for(int i = 0; i < 5; i++)
        {
            if (s_ue.mousebuttons[i] == BUTTON_CLICK)
                s_ue.mousebuttons[i] = BUTTON_HOLD;
        }
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYUP:
            {
                Key sym = GetKeyFromKeyCode(event.key.keysym.sym, event.key.keysym.mod & KMOD_SHIFT);
                if (sym == KEY_UNKNOWN) break;
                s_ue.keymap[sym] = BUTTON_RELEASE;

                if ( s_keyhandlers.size() > 0 )
                {
				    const KeyHandler & cb = s_keyhandlers.top();
                    if( s_echo && cb)
                    {
                        cb(Key(sym), s_ue.keymap[sym]);
                    }
                }
            }
                break;
            case SDL_KEYDOWN:
            {
                Key sym = GetKeyFromKeyCode(event.key.keysym.sym, event.key.keysym.mod & KMOD_SHIFT);
                if (sym == KEY_UNKNOWN) break;
                ButtonState & state = s_ue.keymap[sym];
                if (state == BUTTON_CLICK || state == BUTTON_HOLD)
                    state = BUTTON_HOLD;
                else
                    state = BUTTON_CLICK;
                if ( s_keyhandlers.size() )
                {
                    const KeyHandler & cb = s_keyhandlers.top();
                    if( s_echo && cb)
                    {
                        cb(Key(sym), s_ue.keymap[sym]);
                    }
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
                //align to pixel size
                s_ue.mousex = event.motion.x;
                s_ue.mousey = event.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                s_ue.mousebuttons[event.button.button-1] = BUTTON_CLICK;
                break;
            case SDL_MOUSEBUTTONUP:
            
                s_ue.mousebuttons[event.button.button-1] = BUTTON_RELEASE;
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
        SDL_SetRenderTarget(s_renderer, 0);
        SDL_SetRenderDrawColor(s_renderer, s_clearColor.r, s_clearColor.g, s_clearColor.b, s_clearColor.a);
        SDL_RenderClear(s_renderer);
        SDL_Rect drect = { 0,0, s_windowW,s_windowH };
        SDL_RenderCopy( s_renderer, s_textures[s_target], NULL, &drect );
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
        int sw = (int)(w * s_windowScale);
        int sh = (int)(h * s_windowScale);
        SDL_SetWindowSize(s_window, sw, sh);
        SDL_GetWindowSize(s_window, &s_windowW, &s_windowH);
        if (s_target)
        {
            DestroyTexture(s_target);
        }
        s_target = CreateTexture(w, h, TEXTURE_TARGET);
        
    }
    
    int CreateTexture(int width, int height,TextureMode mode)
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
        SDL_TextureAccess access = SDL_TEXTUREACCESS_STATIC;
        switch ( mode )
        {
        case TEXTURE_LOCKABLE: access = SDL_TEXTUREACCESS_STREAMING;
            break;
        case TEXTURE_TARGET: access = SDL_TEXTUREACCESS_TARGET;
            break;
        case TEXTURE_STATIC: access = SDL_TEXTUREACCESS_STATIC;
            break;
        }
        s_textures[i] = SDL_CreateTexture(s_renderer, SDL_PIXELFORMAT_BGRA8888, access, width, height);
        SetTextureBlendMode(i, BLEND_MIX);
        return i;
    }

    void DestroyTexture(int textureId)
    {  
        CHECK_TEXTURE(textureId )
        SDL_Texture* texture = s_textures[textureId];
        s_textures[textureId] = 0;
        SDL_DestroyTexture(texture);
    }
    //if texture is static

    void UpdateTexture( int textureId, const Color * colors, int width, const Rect & rect )
    {
        CHECK_TEXTURE( textureId );
        const SDL_Rect & sdlrect = { rect.x, rect.y, rect.w, rect.h };
        SDL_UpdateTexture( s_textures[textureId], &sdlrect, colors, width * sizeof( Color ) );
    }

    //If using streaming 
    Color * LockTexture(int textureId, const Rect & region)
    {
        CHECK_TEXTURE(textureId )

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
        CHECK_TEXTURE(textureId )
        SDL_UnlockTexture(s_textures[textureId]);
    }

    Color * LoadPixelsFromFile(const std::string path, int &w, int &h)
    {
        int bpp = 0; //force 4 bpp
        unsigned char *data = stbi_load(path.c_str(), &w, &h, &bpp, 0);
        if ( data == 0 ) return 0;
        Color * pixels = new Color[w*h];
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                int i = y * w + x;
                Color* c = &pixels[i];
                unsigned char* pixel = data + i * bpp;
                c->r = pixel[0];
                c->g = pixel[1];
                c->b = pixel[2];
                c->a = bpp == 4 ? pixel[3] : 255;
            }
        }
        stbi_image_free(data);
        return pixels;
    }


    void Clear(const Color& color)
    {
        ClearTexture(s_target, color);
    }
    SDL_BlendMode GetSDLBlendMode(BlendMode mode)
    {
        switch (mode)
        {

        case BLEND_NONE: return SDL_BLENDMODE_NONE;
        case BLEND_ADD: return SDL_BLENDMODE_ADD;
        case BLEND_MULTIPLY: return SDL_BLENDMODE_MUL;
        case BLEND_MIX: return SDL_BLENDMODE_BLEND;
        }
    }
   
    void SetTextureBlendMode(int textureId, BlendMode mode)
    {
        SDL_BlendMode sdlmode = GetSDLBlendMode(mode);
        SDL_SetTextureBlendMode(  s_textures[textureId], sdlmode );
    
    }

    void SetDrawBlendMode(BlendMode mode)
    {
        SetTextureBlendMode(  s_target, mode );
    }
    


    void ClearTexture(int textureId, const Color& color, BlendMode mode)
    {
        CHECK_TEXTURE(textureId);

        SDL_SetRenderTarget(s_renderer, s_textures[textureId] );
        SDL_BlendMode sdlmode;
        SDL_GetRenderDrawBlendMode(s_renderer, &sdlmode);
        SDL_SetRenderDrawBlendMode(s_renderer, GetSDLBlendMode(mode));

        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
        SDL_SetRenderDrawBlendMode(s_renderer, sdlmode);
        SDL_RenderClear(s_renderer);
    }

    void Blit(int srcTextureId, int destTextureId, const Rect & src, const Rect & dest)
    {
        CHECK_TEXTURE(srcTextureId )
        CHECK_TEXTURE(destTextureId )
        const SDL_Rect & srcrect = { src.x, src.y, src.w, src.h };
        const SDL_Rect & destrect = { dest.x, dest.y,dest.w,dest.h};
        SDL_SetRenderTarget(s_renderer, s_textures[destTextureId] );
        SDL_RenderCopy( s_renderer, s_textures[srcTextureId], &srcrect, &destrect );        
    }
    void MultiplyTexture(int textureId, const Color & color)
    {

        CHECK_TEXTURE(textureId )
        SDL_Texture * texture = s_textures[textureId];
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture, color.a);        
    }
    
    void DrawTexture(int textureId, const Rect & src, const Rect & dest)
    {
        Blit(textureId, s_target, src,  dest);
    }
    void DrawTextureRect(int srcTexture,const Color& color, const Rect& rect, bool filled)
    {
        SDL_SetRenderTarget(s_renderer, s_textures[srcTexture]);
        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
        SDL_Rect sdlrect = { rect.x, rect.y,rect.w,rect.h };
        if (filled)
            SDL_RenderFillRect(s_renderer,&sdlrect);
        else{
            
            SDL_RenderDrawRect(s_renderer,&sdlrect);
            //there is currently a bug in sdl draw rect that misses the bottom-right most pixel
            SDL_RenderDrawPoint(s_renderer, rect.x+rect.w-1,rect.y+rect.h-1);
        }
    }

    void DrawLine(const Color& color, int x1, int y1, int x2, int y2)
    {
        SDL_SetRenderTarget(s_renderer, s_textures[s_target]);
        SDL_SetRenderDrawColor(s_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(s_renderer, x1, y1, x2, y2);
    }

    void DrawRect(const Color& color, const Rect & rect, bool filled)
    {
        DrawTextureRect(s_target, color, rect, filled);
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
        x = s_ue.mousex / s_windowScale;
        y = s_ue.mousey / s_windowScale;

        x = (int)(( x / s_alignX)* s_alignX);
        y = (int)(( y / s_alignY)* s_alignY);

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
        w = (int)(s_windowW / s_windowScale);
        h = (int)(s_windowH / s_windowScale);
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

    Key GetKeyFromKeyCode(SDL_Keycode code, bool cap )
    {
        switch (code)
        {
        case SDLK_RETURN    : return KEY_RETURN;
	    case SDLK_ESCAPE    : return KEY_ESCAPE;
	    case SDLK_BACKSPACE : return KEY_BACKSPACE;
        case SDLK_RSHIFT : 
        case SDLK_LSHIFT : return KEY_SHIFT;
        case SDLK_RALT   :
        case SDLK_LALT   : return KEY_ALT;
	    case SDLK_TAB    : return KEY_TAB;
	    case SDLK_SPACE  : return KEY_SPACE;
        case SDLK_QUOTE  : return cap ? KEY_QUOTEDBL  : KEY_QUOTE;
        case SDLK_COMMA  : return cap ? KEY_LESS      : KEY_COMMA;
	    case SDLK_MINUS  : return cap ? KEY_UNDERSCORE: KEY_MINUS;
	    case SDLK_PERIOD : return cap ? KEY_GREATER   : KEY_PERIOD;
        case SDLK_SLASH  : return cap ? KEY_QUESTION  : KEY_SLASH;
	    case SDLK_0 : return cap ? KEY_RIGHTPAREN : KEY_0;
	    case SDLK_1 : return cap ? KEY_EXCLAIM   : KEY_1;
	    case SDLK_2 : return cap ? KEY_AT        : KEY_2;
	    case SDLK_3 : return cap ? KEY_HASH      : KEY_3;
	    case SDLK_4 : return cap ? KEY_DOLLAR    : KEY_4;
	    case SDLK_5 : return cap ? KEY_PERCENT   : KEY_5;
	    case SDLK_6 : return cap ? KEY_CARET     : KEY_6;
	    case SDLK_7 : return cap ? KEY_AMPERSAND : KEY_7;
	    case SDLK_8 : return cap ? KEY_ASTERISK  : KEY_8;
	    case SDLK_9 : return cap ? KEY_LEFTPAREN : KEY_9;
        case SDLK_SEMICOLON    : return cap ? KEY_COLON     : KEY_SEMICOLON;
        case SDLK_EQUALS       : return cap ? KEY_PLUS      : KEY_EQUALS;
	    case SDLK_LEFTBRACKET  : return cap ? KEY_LEFTBRACE : KEY_LEFTBRACKET;
        case SDLK_RIGHTBRACKET : return cap ? KEY_RIGHTBRACE: KEY_RIGHTBRACKET;
        case SDLK_BACKSLASH    : return cap ? KEY_BAR       : KEY_BACKSLASH;
        case SDLK_BACKQUOTE    : return cap ? KEY_TILDA     : KEY_BACKQUOTE;
	    case SDLK_a : return cap ? KEY_A : KEY_a;
	    case SDLK_b : return cap ? KEY_B : KEY_b;
	    case SDLK_c : return cap ? KEY_C : KEY_c;
	    case SDLK_d : return cap ? KEY_D : KEY_d;
	    case SDLK_e : return cap ? KEY_E : KEY_e;
	    case SDLK_f : return cap ? KEY_F : KEY_f;
	    case SDLK_g : return cap ? KEY_G : KEY_g;
	    case SDLK_h : return cap ? KEY_H : KEY_h;
	    case SDLK_i : return cap ? KEY_I : KEY_i;
	    case SDLK_j : return cap ? KEY_J : KEY_j;
	    case SDLK_k : return cap ? KEY_K : KEY_k;
	    case SDLK_l : return cap ? KEY_L : KEY_l;
	    case SDLK_m : return cap ? KEY_M : KEY_m;
	    case SDLK_n : return cap ? KEY_N : KEY_n;
	    case SDLK_o : return cap ? KEY_O : KEY_o;
	    case SDLK_p : return cap ? KEY_P : KEY_p;
	    case SDLK_q : return cap ? KEY_Q : KEY_q;
	    case SDLK_r : return cap ? KEY_R : KEY_r;
	    case SDLK_s : return cap ? KEY_S : KEY_s;
	    case SDLK_t : return cap ? KEY_T : KEY_t;
	    case SDLK_u : return cap ? KEY_U : KEY_u;
	    case SDLK_v : return cap ? KEY_V : KEY_v;
	    case SDLK_w : return cap ? KEY_W : KEY_w;
	    case SDLK_x : return cap ? KEY_X : KEY_x;
	    case SDLK_y : return cap ? KEY_Y : KEY_y;
	    case SDLK_z : return cap ? KEY_Z : KEY_z;
        case SDLK_UP    : return KEY_UP;
        case SDLK_DOWN  : return KEY_DOWN;
        case SDLK_LEFT  : return KEY_LEFT;
        case SDLK_RIGHT : return KEY_RIGHT;
        default: return KEY_UNKNOWN;
        }
    }
} // namespace Engine