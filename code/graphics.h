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
    //
    //class Image
    //{
    //public:
    //    Image(const std::string & name);

    //    const std::string name;
    //    //nonowning ref to another asset
    //    std::string sheet;

    //    Rect rect;
    //    void draw();

    //private:
    //    //if sheetcache->name != name, then unload old sheet. and load sheet
    //    class Sheet * sheetcache;
    //};

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

        std::string sheet;
        std::string animation; //current animation        
        std::map<std::string, std::vector<Frame>> animframes;
        //position/size
        Rect rect;
        //reload cached sheet from disk.
        void reload();
        void draw();

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
        void blit(const std::string & text, const Rect & dest);
        const int charW,charH;
        const char start;
    };
    
	class TextBox
	{
	public:
		TextBox(int tw, int th, const std::string & text);
        ~TextBox();

        int scrolly; //
        std::string text;
        std::string font;
        int x,y;
        const int w,h;
        
        void reload();
        void draw();
    private:
        int texture; 
        //font sheet to use
        Font * fontcache;
	};
    //Create a Map. Contains a 2d array or 1d indices that map to a 2d tilesheet.
    //reload function memcopies tiles into render texture.
}
