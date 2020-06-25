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
        Sprite(const std::string & name);

        std::string sheet;
        std::map<std::string, std::vector<Frame>> animframes;

        std::string animation;        
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

    //Create a Map. Contains a 2d array or 1d indices that map to a 2d tilesheet.
    //reload function memcopies tiles into render texture.
}
