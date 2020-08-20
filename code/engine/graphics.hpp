#pragma once

#include "core.hpp"
#include "../assets/api.hpp"


//Default Tileset size
//Size fits to bottom of the screen
#define TILESET_W 128
#define TILESET_H 32
//Default Tileset sizes
#define TILE_W 8
#define TILE_H 8
#define TILE_COUNT (TILESET_W/TILE_W * TILESET_H/TILE_H) 

#define MAP_W 128
#define MAP_H 128
//if tile index is -1, then will be clear
#define TILE_CLEAR -1

//Max number of tilesets per map
#define TILESETS_PER_MAP 2
//Default Tileset sizes
#define SPRITE_W 16
#define SPRITE_H 16
#define SPRITE_W_SMALL 8
#define SPRITE_H_SMALL 8
#define SPRITE_COUNT (TILESET_W/SPRITE_W * TILESET_H/SPRITE_W) 
#define SPRITE_SMALL_COUNT (TILESET_W/SPRITE_W_SMALL * TILESET_H/SPRITE_H_SMALL) 

namespace Graphics
{
    struct Renderable
    {
        //TODO - 
        virtual void update() = 0;
        virtual void draw() = 0;
    };

    /*--------------------------- Tileset - -----------------------------------
        Represents an image
    */
    class Tileset : public Asset
    {
    public:
        Tileset(const std::string& name, int w=TILESET_W, int h=TILESET_H);
        ~Tileset();
        
        //reset the data         
        void reset(Color * pixels, int w, int h);
        //call sparingly . if no rect, will update entire. pushes data to gpu
        void update(const Rect & rect = { 0, 0, 0, 0 } ); 
        //given tile rect get index
        int id( const Rect & tile) const;
        //get tile rect of size tx x th 
        Rect tile( int tileId, int tw, int th ) const;


        Color * pixels;
        int w, h;
        int texture;
    };



    /*------------------------------------- Map ------------------------------------
        
    */
    class Map : public Asset
    {
    public:
        //width and height is number of tiles
        Map(const std::string & name, int w=MAP_W, int h=MAP_H, int tilew=TILE_W, int tileh=TILE_H);
        ~Map();
        
        //reset the data and reload
        void reset(char * tiles, int w, int h, int tilew=TILE_W, int tileh=TILE_H); 
        //update texute with tile data
        void update(const Rect & rect = { 0, 0, 0, 0 } ); 
        //update tilesheets and update
        void reload();
        void draw();
        float scale( );
        void zoomTo( float scale, int x, int y );
        void scroll(int dx, int dy);
        //get screenspace tile rect at x y 
        Rect tile(int scrx, int scry);
        //tile xy in map space from screen space
        bool getTileXY(int scrx, int scry, int & tilex, int & tiley);
        void clear();

        //can use 4 tilesets. 
        //tileset index =  index / TILE_COUNT
        std::string tilesets[TILESETS_PER_MAP];

        //The view is viewport into the map texture
        Rect view;
        //xy is screen space position of map. 
        Rect rect;
        
        //do not modify
        //width and height is in tiles
        int w,h, tilew, tileh, worldw, worldh;

        char * tiles;
        
     private:
        Tileset * m_tilesetscache[TILESETS_PER_MAP];
        //TODO - split map into multiple subtextures. Each streamed in on demand. "Super maps"
        int m_texture; 
        //viewport
        float m_scale;

    };

      /*--------------------------- Font ------------------------------------
        font is essentially just a tile set, where each character in the alphabet is just a tile
    */
    class Font : public Tileset
    {
    public:
        Font(const std::string& name, int w, int h, int charW = TILE_W, int charH = TILE_H, char start = ' ');

        //src is textbox in character units
        void blit(int destTexture, const std::string & text, const Rect & dest);

        //do not modify!s
        int charW,charH;
        char start;
        
    };

    // ================================== Runtime graphical components ==================================

    /*--------------------------- -------------  Sprite  ------------------------------------
        Describes a sprite. nothing about runtime info. 
    */
    class Sprite
    {
    public:
        Sprite(int tile, int w = SPRITE_W, int h = SPRITE_H);
        ~Sprite();
        void reload();
        void draw();

        //tile index
        int tile ;
        int x,y;
        int w,h;
        std::string tileset;
        //TODO add animation info

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
		TextBox(int tw, int th, const std::string & text, const std::string & font);
        ~TextBox();

        //clear and redraw texture
        void refresh();
        //update font sheet and refresh texture
        void reload();
        void draw();
        const Font *getFont();
        
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
