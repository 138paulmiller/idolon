#pragma once
#include "core.hpp"

#define FPS_CAP 60
#define FPS_SAMPLE_COUNT 100

enum ButtonState 
{
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_HOLD,//if previously down
};

//Mouse button
enum MouseButton
{
    MOUSEBUTTON_LEFT = 0,
    MOUSEBUTTON_MIDDLE, //if previously down
    MOUSEBUTTON_RIGHT ,
    MOUSEBUTTON_X1 ,
    MOUSEBUTTON_X2 ,
};

//Mouse button
enum BlendMode
{
    BLEND_NONE= 0, //color = newcolor
    BLEND_ADD,
    BLEND_MULTIPLY,
    BLEND_MIX
};

//Mouse button
enum TextureMode
{
    TEXTURE_STATIC = 0, //use update to change pixels
    TEXTURE_LOCKABLE, //locakable. used for multiple updates
    TEXTURE_TARGET //render target
};


namespace Engine
{
    void Startup(int w, int h, float = 1);
    bool Run();
    void Shutdown();
    void SetKeyEcho(bool on);
    //only use in edit/debug mode
    void SetKeyHandler(std::function<void(Key, bool)> cb);
    void AlignMouse(int x, int y);

    // Texture 
    int CreateTexture(int width, int height, TextureMode mode = TEXTURE_STATIC);
    void DestroyTexture(int textureId);
    //if using static texture. width is pitch in color count
    void UpdateTexture( int textureId, const Color* colors, int width, const Rect & rect);
    //Gget handle to pixels in memory
    Color * LockTexture(int textureId, const Rect & region);
    void UnlockTexture(int textureId);
    //multiply color of texture on each texture draw 
    void MultiplyTexture(int textureId, const Color& color);
    void ClearTexture(int textureId, const Color& color, BlendMode mode= BLEND_MIX);
    void SetTextureBlendMode(int textureId, BlendMode mode);


    //Copy data from one texture to another    
    void Blit(int srcTexture, int destTexture, const Rect & src, const Rect & dest);
    void DrawTexture(int textureId, const Rect & src, const Rect & dest);
    void DrawLine(const Color& color, int x1, int y1, int x2, int y2);
    void DrawRect(const Color& color, const Rect& rect, bool filled);
    void ClearScreen(const Color& color = { 255, 0, 0, 0 } );
    void SetDrawBlendMode(BlendMode mode);

    //Getters/setters
    void Resize(int w, int h);
    //size of draw region in "units". Not window size
    void GetSize(int& w, int& h);
    uint32_t GetTime();
    uint32_t GetTimeDeltaMs();

    float GetFPS();

    ButtonState GetKeyState(Key key);
    ButtonState GetMouseButtonState(MouseButton button);
    //Mouse position in window space
    void GetMousePosition(int& x, int& y);
    void GetMouseWheel(int& x, int& y, int& dx, int& dy);

    //Tools 

    //Load and image and save as an asset
    Color * LoadTexture(const std::string path, int &w, int &h);
}