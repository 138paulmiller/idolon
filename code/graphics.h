#pragma once
#include "core.h"
#include "assets.h"
/*
    Basis 2D Render system. 
    Create asset serialization lib. Given a name. load a sprite/sheet/palette. Each asset can have references to each other. 
*/


namespace Graphics
{
    class Sheet : public Asset
    {
        friend class Sprite;
    public:
        
        Sheet(const std::string& name, int w, int h);
        ~Sheet();
        
        //call sparingly 
        void update(const Rect & rect = { 0, 0, 0, 0 } ); 

        Color * const pixels;
        const int w, h;
        const int texture;
    };
    //animation frame
    struct Frame
    {
        //time spent at frame
        float duration;
        //offset in texture
        Rect region;
    };

    class Sprite  : public Asset
    {
    public:
        Sprite(int w, int h, const std::string & name);
        //reload cached sheet from disk.
        void reload();
        void draw();

        std::string sheet;
        std::string animation; //current animation        
        std::map<std::string, std::vector<Frame>> animframes;
        //position/size
        Rect rect;

    private:
        //if sheetcache->name != name, then unload old sheet. and load sheet
        Sheet * sheetcache;
        //current animation
        int iframe;

        float timer;
    };

    class Font : public Sheet
    {
    public:
        Font(const std::string& name, int w, int h, int charW, int charH, char start);
        //src is textbox in character units
        void blit(int destTexture, const std::string & text, const Rect & dest);

        const int charW,charH;
        const char start;
    };
    //TODO Create a texture that is sizeof(boxtexture)/fontw
    // this contains the colors. then perform additive blending
	class TextBox
	{
	public:
        // w and height are number of chars
		TextBox(int tw, int th, const std::string & text);
        ~TextBox();
        void refresh();
        void reload();
        void draw();
        
        Color textColor;
        //Fill should be texture!
        bool filled; //
        Color fillColor;
        std::string text;
        std::string font;
        int scrolly; //
        int x,y;
        int w,h;
        int tw,th;
        bool visible;

    private:
        int texture; 
        //font sheet to use
        Font * fontcache;
	};
    //Create a Map. Contains a 2d array or 1d indices that map to a 2d tilesheet.
    //reload function memcopies tiles into render texture.
}
