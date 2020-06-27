#pragma once
#include "core.h"

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
typedef void(*EchoCallback)(Key key);

namespace Engine
{
    void Startup(int w, int h, float = 1/5.0);
    bool Run();
    void Shutdown();
    void SetEcho(bool on);
    //only use in edit/debug mode
    void SetEchoHandle(EchoCallback cb);

    // Texture 
    int CreateTexture(int width, int height);
    void DestroyTexture(int textureId);
    //update the pixels of texure
    Color * LockTexture(int textureId, const Rect & region);
    void UnlockTexture(int textureId);
    
    void Blit(int srcTexture, int destTexture, const Rect & src, const Rect & dest);
    void DrawTexture(int textureId, const Rect & src, const Rect & dest);
    void DrawLine(const Color& color, int x1, int y1, int x2, int y2);
    void DrawRect(const Color& color, const Rect& rect, bool filled);

    //void DrawRect(const Rect & dest, bool filled)
    
    //Getters/setters
    void Resize(int w, int h);
    //size of draw region. windows may be larger
    void GetSize(int& w, int& h);
    uint32_t GetTime();
    uint32_t GetTimeDeltaMs();

    float GetFPS();

    ButtonState GetKeyState(Key key);
    ButtonState GetMouseButtonState(MouseButton button);
    void GetMousePosition(int& x, int& y);
    void GetMouseWheel(int& x, int& y, int& dx, int& dy);

    //Tools 

    //Load and image and save as an asset
    Color * LoadTexture(const std::string path, int &w, int &h);
}