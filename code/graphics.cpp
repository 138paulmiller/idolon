#include "pch.hpp"
#include "core.hpp"
#include "assets.hpp"
#include "engine.hpp"
#include "graphics.hpp"

#include <sstream>

namespace Graphics
{


    Tileset::Tileset( const std::string& name, int w, int h )
        :Asset( name ), w( w ), h( h ),
        pixels( new Color[( int ) ( w * h )] ),
        texture( Engine::CreateTexture( w, h ) )
    {
        memset( pixels, 255, w * h * sizeof( Color ) );
        update();
    }

    Tileset::~Tileset()
    {
        delete[]pixels;
        Engine::DestroyTexture( texture );
    }

    void Tileset::update( const Rect& rect )
    {
        ASSERT( rect.x >= 0 && rect.w + rect.x <= w
            && rect.y >= 0 && rect.h + rect.y <= h,
            "Engine: Invalid rect" );

        if ( rect.w == 0 || rect.h == 0 )
        {
            Engine::UpdateTexture( texture, pixels, w, { 0,0,w,h } );
            //Color * data = Engine::LockTexture( texture, { 0,0,w,h } );
            //memcpy(data, pixels, w * h * sizeof(Color));
        }
        else
        {
            Engine::UpdateTexture( texture, &pixels[rect.y * w + rect.x], w, rect );
            //Color * data = Engine::LockTexture( texture, rect );
            //for(int y = 0; y < rect.h; y++)
            //    for(int x = 0; x < rect.w; x++)
            //        data[y * w  + x] = pixels[(rect.y+y) * w  + rect.x + x];
        }
        //Engine::UnlockTexture(texture);

    }
    int Tileset::id( const Rect& tile ) const
    {
        const int tw = w / tile.w;
        const int tx = tile.x / tile.w;
        const int ty = tile.y / tile.h;
        return ty * tw + tx;
    }

    Rect Tileset::tile( int index, int tw, int th ) const
    {
        const int x = ( index % ( w / tw ) ) * tw;
        const int y = ( index / ( w / tw ) ) * th;
        return { x,y, tw, th };
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    /*
    Split map into 128x128 textures. Render these to the screen
    */
    //width and height is number of tiles
    Map::Map( const std::string& name, int w, int h, int tilew, int tileh )
        :Asset( name ),
        w( w ), h( h ),
        m_scale(1.0),
        tilew( tilew ), tileh( tileh ),
        worldw( w * tilew ), worldh( h * tileh ),
        m_tilesetcache( 0 ),
        m_texture( Engine::CreateTexture( worldw, worldh, TEXTURE_TARGET ) ),
        tiles( new char[w * h] ),
        //by default look for sheet with same name
        tileset( name )

    {
        memset( tiles, 0, (int)(w * h) );
        int screenw, screenh;
        Engine::GetSize(screenw,screenh);
        view = { 0, 0, screenw, screenh };
        rect = { 0, 0, screenw, screenh };
    }

    Map::~Map()
    {
        if(m_tilesetcache)
            Assets::Unload<Tileset>(m_tilesetcache->name);

        Engine::DestroyTexture( m_texture );
        delete tiles;

    }

    float Map::scale()
    {
        return m_scale;
    }

    void Map::zoomTo(float scale, int x, int y)
    {
        //delta
        scale *= m_scale ;
        if ( scale > 1 ) 
        {
            scale = 1;
            return;
        }
        else if ( scale < 0.125f ) 
        {
            scale = 0.125f;
            return;
        }
        float delta =  m_scale - scale;
        m_scale = scale;

        int px = view.x;
        int py = view.y;

        view.x = (delta * x ) ;
        view.y = (delta * y ) ;


        view.w = rect.w * m_scale;
        view.h = rect.h * m_scale;

        if(view.w < TILE_W) 
            view.w = TILE_W;
        else if(view.w > rect.w) 
            view.w = rect.w;

        if(view.h < TILE_H) 
            view.h = TILE_H;
        else if(view.h > rect.h) 
            view.h = rect.h;
    
        scroll( view.x - px, view.y - py );
    }

    void Map::scroll( int dx, int dy )
    {

        view.x += dx * m_scale;
        view.y += dy * m_scale;
    	
        if(view.x < 0) 
		    view.x = 0;
	    else if(view.x+view.w > worldw) 
		    view.x = worldw-view.w;
	    
        if(view.y < 0) 
		    view.y = 0;
	    else if(view.y+view.h > worldh) 
		    view.y = worldh-view.h;
	
    }
    void Map::reload()
    {

        if(m_tilesetcache)
            Assets::Unload<Tileset>(m_tilesetcache->name);

        m_tilesetcache = Assets::Load<Tileset>(tileset);
        update({0,0,w,h}); 

    }
    //rect is in tile space
    void Map::update(const Rect & rect )
    {
        if(!m_tilesetcache) return;
        const Rect& region = { rect.x, rect.y, rect.w == 0 ? w : rect.w, rect.h == 0 ? h : rect.h };


        for(int y = region.y; y < (region.y+region.h); y++)
            for(int x = region.x; x < (region.x+region.w); x++)
            {
                const int tile = tiles[ y * w + x]; 
                const Rect & src = m_tilesetcache->tile(tile, tilew, tileh);
                const Rect & dest = { x*tilew, y*tileh, tilew, tileh };
                Engine::Blit(m_tilesetcache->texture, m_texture, src, dest);                
            }
    }

    void Map::clear()
    {
        Engine::ClearTexture( m_texture, { 0,0,0,0});
    }
    void Map::draw()
    {
        if(!m_tilesetcache) return;

        Engine::DrawTexture( m_texture, view, rect );

    }

    Rect Map::tile(int scrx, int scry)
    {
        if ( ( scrx < rect.x || scrx > rect.x + rect.w )
            || ( scry < rect.y || scry > rect.y + rect.h ) )
            return {-1, -1, -1, -1} ;
         //tile xy in screen space 
        int tilex =  view.x/m_scale + scrx - rect.x;
        int tiley =  view.y/m_scale + scry - rect.y;

        //screen space tile width/height
        const int scrTW = tilew/m_scale;
        const int scrTH = tileh/m_scale;
        int alignscrx = tilex/scrTW*scrTW - view.x/m_scale + rect.x;
        int alignscry = tiley/scrTH*scrTH - view.y/m_scale + rect.y;

        return {  alignscrx, alignscry, scrTW,  scrTH };
    }
    bool Map::getTileXY( int scrx, int scry, int& tilex, int& tiley )
    {
        if ( ( scrx < rect.x || scrx > rect.x + rect.w )
            || ( scry < rect.y || scry > rect.y + rect.h ) )
            return false;
     
        const int scrTH = tileh/m_scale;
        tilex =  ( view.x/m_scale + scrx - rect.x )/ tilew/m_scale ;
        tiley =  ( view.y/m_scale + scry - rect.y )/ tileh/m_scale ;

        return true;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////

    Sprite::Sprite(int tileId, int w, int h)
        :x(0), y(0), w(w), h(h), 
        tileset(""), tile(tileId), 
        m_tilesetcache(0)
    {

    }
    Sprite::~Sprite()
    {
        if(m_tilesetcache)
            Assets::Unload<Tileset>(m_tilesetcache->name);
    }

     void Sprite::reload()
    {
        if(m_tilesetcache)
            Assets::Unload<Tileset>(m_tilesetcache->name);
        m_tilesetcache = Assets::Load<Tileset>(tileset);
    }

    void Sprite::draw()
    {
        if ( !m_tilesetcache ) return;
        const Rect& src = m_tilesetcache->tile(tile, w, h);
        Engine::DrawTexture( m_tilesetcache->texture, src, { x,y,w,h } );
    }

    ///////////////////////////////////////////////////////////////////////////////

    Font::Font(const std::string& name, int w, int h, int charW, int charH, char start)
         :Tileset(name, w, h), 
         charW(charW), charH(charH), 
         start(start)
    { 
    }

    void Font::blit(int destTexture, const std::string & text, const Rect & dest)
    {
        int c, sx,sy;
        const int srcW = w / charW;  
        const int destW = dest.w / charW;     
        const int destH = dest.h / charH;     
        int scrolly = 0;
        int dx = 0, dy = 0;
        for(int i = 0; i < text.size(); i++)
        {
            int s =  text[i];
            if(s == '\n')
            {
                i++;
                if(i== text.size()) break;
                scrolly++;
                s =  text[i];
                dx = 0;
                dy++;
            }
            if(dx == destW)
            {
                dx = 0;
                dy++;
            }
            c =  s - start;
            sx = (c % srcW);
            sy = (c / srcW) ;            
            Rect src = { 
                sx * charW, sy * charH, 
                charW, charH 
            } ;
            Rect pos = { 
                dest.x + dx * charW, 
                dest.y + dy * charH, 
                charW, charH 
            };   
            if(dy > destH) 
                break;
            Engine::Blit(texture, destTexture, src, pos);
            dx++;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////
    
    TextBox::TextBox(int tw, int th, const std::string & text)
        :text(text),
        font(DEFAULT_FONT),
        m_texture (0),
        x(0), y(0), 
        w(0), h(0),
        tw(tw), th(th),
        scrolly(0),filled(false),
        textColor({255,255,255,255}),
        fillColor({0,0,0,0}),
        visible(true),
        borderX(0),
        borderY(0),
        m_fontcache(0)
    {
    }
    TextBox::~TextBox()
    {
        if(m_fontcache)
            Assets::Unload<Font>(m_fontcache->name);

        Engine::DestroyTexture(m_texture);
    }

    void TextBox::draw()
    {
        if (!visible) return;
        if(!m_fontcache) return;   
        Engine::DrawTexture(m_texture, {0,0,w,h}, {x,y,w,h});
    }

    void TextBox::refresh()
    {
        Engine::ClearTexture(m_texture, fillColor);
        m_fontcache->blit(m_texture, text, { borderX, borderY , w, h });
        for(int y =0 ; y < m_fontcache->h; y++ )
            for(int x =0 ; x < m_fontcache->w; x++ )
            {
                //color only text 
                Color & color = m_fontcache->pixels[y * m_fontcache->w + x];
                if(color == fillColor)
                    color = textColor;
            }

    }
    void TextBox::reload()
    {

        if(m_fontcache)
            Assets::Unload<Font>(m_fontcache->name);
        m_fontcache = Assets::Load<Font>(font);

        if(!m_fontcache) return;
        //only do if texture w/h changes
        if(m_texture) Engine::DestroyTexture(m_texture);
        w = tw * m_fontcache->charW + borderX*2;
        h = th * m_fontcache->charH + borderY*2;
        m_texture = Engine::CreateTexture(w, h, TEXTURE_TARGET);
        if(!filled)
            Engine::SetTextureBlendMode(m_texture, BLEND_ADD);
        refresh();
    }
} // namespace Graphics