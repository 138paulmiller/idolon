#pragma once

#include "core.hpp"
#include "../assets/api.hpp"



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
        void draw();
        float scale( );
        //view w,h
        void resize( int w, int h );
        void zoomTo( float scale, int x, int y );
		void scroll(int dx, int dy);
        //get screenspace tile rect at x y 
        Rect getTileRect(int scrx, int scry)const ;
        //tile xy in map space from screen space
        bool getTileXY(int scrx, int scry, int & tilex, int & tiley)const ;
        
        const Tileset * getTileset(int index ) const ;
        void setTileset(int index, const Tileset * tileset ) ;

        
        void clear();
		void clampView();


        //The view is viewport into the map texture
        Rect view;
        //xy is screen space position of map. 
        Rect rect;
        
        //do not modify
        //width and height is in tiles
        int w,h, tilew, tileh, worldw, worldh;

        //tileset index =  index / TILE_COUNT
        char * tiles;
        
     private:
        //can use 4 tilesets. 
        const Tileset * m_tilesets[TILESETS_PER_MAP];
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
        //to make ascii pritanable start be ' '
        Font(const std::string& name, int w, int h, int charW = TILE_W, int charH = TILE_H, char start = 0 );

        //src is textbox in character units
        // if isEscaped, will handle newlines and replace tabs
        void blit(int destTexture, const std::string & text, const Rect & dest, int scrolly, bool isEscaped );

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

        void draw();

        //tile index
        int tile ;
        int x,y;
        int w,h;

        void setTileset(const Tileset * tileset);
        const Tileset * getTileset();
        //TODO add animation info

    private:
        const Tileset * m_tileset;
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
        Rect view;
        
        bool filled; 
        bool visible;
        bool escaped;
    private:
        int m_texture; 
        //font sheet to use
        Font * m_fontcache;
        int m_textureW, m_textureH; 
	};

}
