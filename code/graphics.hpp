#pragma once

#include "core.hpp"
#include "assets.hpp"

namespace Graphics
{
    /*--------------------------- Tileset - -----------------------------------
        Represents an image
    */
    class Tileset : public Asset
    {
        friend class Sprite;
    public:
        
        Tileset(const std::string& name, int w, int h);
        ~Tileset();
        
        //call sparingly . if no rect, will update entire. push data to gpu
        void update(const Rect & rect = { 0, 0, 0, 0 } ); 

        Color * const pixels;
        const int w, h;
        const int texture;
    };

    /*--------------------------- Tileset - -----------------------------------
        animation frame
    */
    struct Frame
    {
        //time spent at frame
        float duration;
        //offset in texture
        Rect region;
    };

    /*--------------------------- Sprite ------------------------------------
        Describes a sprite. nothing about runtime info. 
    */

    class Sprite  : public Asset
    {
    public:
        Sprite(const std::string & name, int w, int h);
        void reload();
        void draw(const Rect & src, const Rect & dest);
        
        int w,h;
        std::string sheet;
        std::map<std::string, std::vector<Frame> > animframes;

    private:
        Tileset * m_tilesetcache;
    };
  
    /*--------------------------- SpriteInstance ------------------------------------
        Sprites are instanced. The sprite description 
    */
    class SpriteInstance
    {
    public:
        SpriteInstance(const std::string & sprite);
        //reload cached sheet from disk.
        void draw();

        std::string sprite;
        std::string animation; //current animation        
        //current position/size
        Rect rect;

    private:
        Sprite * m_spritecache;
        int m_iframe;
        float m_timer;
    };

    /*--------------------------- Map ------------------------------------
        contains a list of references to some sprites and their positions
        on load. must override saved sprite positions
    */
    class Map : public Asset
    {
    public:
        //width and height is number of tiles
        Map(int w, int h, const std::string & name);
        void reload();
        void draw();

        std::string sheet;
        //viewport
        Rect view;

        void removeSprite(const std::string & name);
        void addSprite(const std::string & spriteName, const std::string & instanceName, int x, int y);
    private:


        std::vector<int> m_tiles;
        //map to sprite instance (cache )
        std::unordered_map<std::string, SpriteInstance*> m_sprites;
        
        Tileset * sheetcache;
        //TODO - split map into multiple subtextures. Each streamed in on demand. "Super maps"
        int m_texture; 

    };
    /*--------------------------- Font ------------------------------------
        font is essentially just a tile set, where each character in the alphabet is just a tile
    */
    class Font : public Tileset
    {
    public:
        Font(const std::string& name, int w, int h, int charW, int charH, char start);
        //src is textbox in character units
        void blit(int destTexture, const std::string & text, const Rect & dest);

        const int charW,charH;
        const char start;
    };

    /*--------------------------- Textbox ------------------------------------
        TODO Create a texture that is sizeof(boxtexture)/fontw
            this contains the colors. then perform additive blending
    */
	class TextBox
	{
	public:
        // tw and th are number of chars
		TextBox(int tw, int th, const std::string & text);
        ~TextBox();
        //update test box
        void refresh();
        //update font and refresh
        void reload();
        void draw();
        
        Color textColor;
        Color fillColor;
        std::string text;
        std::string font;
        int scrolly; //
        int x,y;
        int w,h;
        int tw,th;
        int borderX, borderY;

        bool filled; 
        bool visible;

    private:
        int m_texture; 
        //font sheet to use
        Font * m_fontcache;
	};

}
