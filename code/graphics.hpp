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
        //given tile rect get index
        int id( const Rect & tile) const;

        //get tile rect of size tx x th 
        Rect tile( int tileId, int tw, int th ) const;
        Color * const pixels;
        const int w, h;
        const int texture;
    };



    /*--------------------------- Map ------------------------------------
        contains a list of references to some sprites and their positions
        on load. must override saved sprite positions
    */
    class Map : public Asset
    {
    public:
        //width and height is number of tiles
        Map(const std::string & name, int w, int h, int tilew, int tileh);
        ~Map();
        
        void update(const Rect & rect = { 0, 0, 0, 0 } ); 
        void reload();
        void draw();
        //pixel space
        void setView(int x, int y, int w, int h);
        const Rect & getView();
        void zoom(const float delta);
        void scroll(int dx, int dy);

        std::string sheet;
        const int w,h, tilew, tileh, worldw, worldh;
        char * const tiles;
     private:
        Tileset * m_tilesetcache;
        //TODO - split map into multiple subtextures. Each streamed in on demand. "Super maps"
        const int m_texture; 
        //viewport
        Rect m_view;

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

    // ------------ Drawables. These are runtime. Not serialized assets  ------------

    /*--------------------------- Sprite ------------------------------------
        Describes a sprite. nothing about runtime info. 
    */
    class Sprite
    {
    public:
        Sprite(int tile, int w, int h);
        void reload();
        void draw();
        //tile index
        int tile ;
        int x,y;
        int w,h;
        std::string sheet;

    private:
        Tileset * m_tilesetcache;
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
