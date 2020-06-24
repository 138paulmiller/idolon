#pragma once
#include "pch.h"
#include "assets.h"
/*
    Basis 2D Render system. 
    Create asset serialization lib. Given a name. load a sprite/sheet/palette. Each asset can have references to each other. 
*/


namespace Graphics
{
    class Sheet : public Asset
    {
    public:
        
        Sheet(const std::string& name, int w, int h);
        ~Sheet();
        
        //call sparingly 
        void update(); 
        void use();

        Color * const pixels;
        const int w, h;

    private:
        int texture;
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

        void update();
        void draw();

    private:
        //if sheetcache->name != name, then unload old sheet. and load sheet
        class Sheet * sheetcache;
        //current animation
        int iframe;

        float timer;
    };

}
