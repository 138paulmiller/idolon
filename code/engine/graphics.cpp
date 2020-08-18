
#include "fs.hpp"
#include "core.hpp"
#include "engine.hpp"
#include "graphics.hpp"


namespace Graphics
{


    Tileset::Tileset()
        : w( 0 ), h( 0 )
        , pixels( 0)
        , texture( -1 )
    {
    }

    Tileset::Tileset( const std::string& name, int w, int h )
        : Asset( name )
        , w( w ), h( h )
        , pixels( 0)
        , texture( -1 )
    {
        const int size = w*h * sizeof( Color );
        Color * empty = new Color[size];
        memset( empty, 0, size);
        reset(empty, w, h);
    }

    Tileset::~Tileset()
    {
        delete[]pixels;
        Engine::DestroyTexture( texture );
    }

    bool Tileset::deserialize(std::istream & in)
    {
        try 
        {
            std::getline( in, name, '\n' ); 
            FS::ReplaceAll(name, "\r", "");

            in >> w >> h;
            int blocksize;
            in >> blocksize ;
            char * imagedata = new char[blocksize]; 
            in.read(imagedata, blocksize);
            reset( (Color*)imagedata, w , h);
        }
        catch (...)
        {
            LOG("Assets: Failed to load tileset %s\n", name.c_str());
            return 0;
        }
        return 1;
    }
    
    void Tileset::serialize(std::ostream & out) const 
    {
        out << name << std::endl;
        //TODO - verify endian-ness!
        int blocksize = sizeof(Color) * w * h;
        out << w << ' ' << h << ' ' << blocksize;
        out.write((char*)pixels, blocksize);
        LOG("Assets: Saved tileset\n");
    }

    void Tileset::reset(Color * newpixels, int newW, int newH)
    {
        w = newW;
        h = newH;
        if(pixels)
            delete[]pixels;
        pixels = newpixels;
        if(texture != -1)
            Engine::DestroyTexture(texture);
        texture =  Engine::CreateTexture( w, h );
        update();
    }

    void Tileset::update( const Rect& rect )
    {
        if(pixels == 0 ) 
            return;
        if (!( rect.x >= 0 && rect.w + rect.x <= w
            && rect.y >= 0 && rect.h + rect.y <= h))
            return;

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
    TODO - Split map into 128x128 textures. Render these to the screen
    */
    Map::Map()
        :m_texture( -1 )
        ,w( 0 ), h( 0 )
        ,tilew( 0 ), tileh( 0 )
        ,tiles( 0 )      
    {
        for ( int i = 0; i < TILESETS_PER_MAP; i++ )
        {
            tilesets[i] = "";
            m_tilesetscache[i] = 0;
        }

    }

    //width and height is number of tiles
    Map::Map( const std::string& name, int w, int h, int tilew, int tileh )
        :Asset( name )
        ,w( w ), h( h )
        ,tilew( tilew ), tileh( tileh )
        ,m_texture( -1 )
        ,tiles( 0 )

    {
        for ( int i = 0; i < TILESETS_PER_MAP; i++ )
        {
            tilesets[i] = "";
            m_tilesetscache[i] = 0;
        }

        char *const initial = new char[w * h];
        memset(initial, 0, sizeof(char )*w * h );
        reset(initial, w, h, tilew, tileh);
    }

    Map::~Map()
    {
        for ( int i = 0; i < TILESETS_PER_MAP; i++ )
        {
            if(m_tilesetscache[i] && tilesets[i] != "")
                Assets::Unload<Tileset>(m_tilesetscache[i]->name);
        }

        Engine::DestroyTexture( m_texture );
        delete tiles;

    }

    bool Map::deserialize( std::istream & in)
    {
        try 
        {
            std::getline( in, name, '\n' ); 
            FS::ReplaceAll(name, "\r", "");

            for ( int i = 0; i < TILESETS_PER_MAP; i++ )
            {
                std::getline( in, tilesets[i], '\n' ); 
                FS::ReplaceAll(tilesets[i], "\r", "");
            }
            in >> w >> h >> tilew >> tileh;

            //TODO - verify endian-ness!
            int blocksize;
            in >> blocksize ;
            char * tiledata = new char[blocksize]; 
            in.read(tiledata, blocksize);
            reset(tiledata, w,h, tilew, tileh);

            LOG("Assets: Loaded map %s\n", name.c_str());

        }
        catch (...)
        {
            LOG("Assets: Failed to load map %s\n", name.c_str());
            return 0;
        }
        return 1;
    }   
    
    void Map::serialize(std::ostream & out) const
    {

        out << name << std::endl;
        for(int i = 0 ; i < TILESETS_PER_MAP; i++ )
        {
            out << tilesets[i] << std::endl;
        }
        //TODO - verify endian-ness!
        int blocksize =  w * h;
        out << w << ' ' << h << ' ';
        out << tilew << ' ' << tileh << ' ';
        out << blocksize;
        out.write((char * )tiles, blocksize);
        LOG("Assets: Saved map\n");
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
        m_scale = scale;
        
        scroll( x - view.x, y - view.y );

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

    void Map::reset(char * newTiles, int newW, int newH, int newTilew, int newTileh)
    {
        w = newW;
        h = newH;
        tilew = newTilew;
        tileh = newTileh;

        worldw = w * tilew;
        worldh = h * tileh;

        if(m_texture != -1)
            Engine::DestroyTexture(m_texture);
        m_texture = Engine::CreateTexture( worldw, worldh, TEXTURE_TARGET );

        if(tiles)
            delete tiles;
        tiles = newTiles;

        m_scale = 1.0;
        int screenw, screenh;
        Engine::GetSize(screenw,screenh);
        view = { 0, 0, screenw, screenh };
        rect = { 0, 0, screenw, screenh };    
        reload();
    }
    

    void Map::reload()
    {
        for ( int i = 0; i < TILESETS_PER_MAP; i++ )
        {
            if(m_tilesetscache[i] && tilesets[i] != "")
                Assets::Unload<Tileset>(m_tilesetscache[i]->name);
            m_tilesetscache[i] = Assets::Load<Tileset>(tilesets[i]);
        }
        update({0,0,w,h}); 

    }
    //rect is in tile space
    void Map::update(const Rect & rect )
    {
        const Rect& region = { rect.x, rect.y, rect.w == 0 ? w : rect.w, rect.h == 0 ? h : rect.h };

        for(int y = region.y; y < (region.y+region.h); y++)
            for(int x = region.x; x < (region.x+region.w); x++)
            {
                char tile = tiles[ y * w + x];
                const int tilesetIndex = floor(tile / TILE_COUNT);
                tile = tile - tilesetIndex * TILE_COUNT;
                if(tile >= TILE_COUNT) 
                    return;
                Tileset* tileset = m_tilesetscache[tilesetIndex];

                const Rect & dest = { x*tilew, y*tileh, tilew, tileh };
                //clear portion if clear color or null tileset
                if (tileset == 0 || tile == TILE_CLEAR )
                {
                    Engine::DrawTextureRect(m_texture, {0,0,0,0}, dest, true);
                }
                else
                { 
                    const Rect& src = tileset->tile( tile, tilew, tileh );
                    Engine::DrawTextureRect(m_texture, {0,0,0,0}, dest, true);
                    Engine::Blit( tileset->texture, m_texture, src, dest );
                }

            }
    }

    void Map::clear()
    {
        Engine::ClearTexture( m_texture, { 0,0,0,0});
    }
    void Map::draw()
    {
        Engine::DrawTexture( m_texture, view, rect );
    }

    Rect Map::tile(int scrx, int scry)
    {
         //tile xy in screen space 
        int tilex;
        int tiley;
        if(!getTileXY(scrx, scry, tilex, tiley )) 
            return {-1, -1, -1, -1} ;

        //screen space tile width/height
        const int scrTW = tilew/m_scale;
        const int scrTH = tileh/m_scale;
        const int alignscrx = rect.x + tilex*scrTW - (view.x/m_scale);
        const int alignscry = rect.y + tiley*scrTH - (view.y/m_scale);

        return {  alignscrx, alignscry, scrTW,  scrTH };
    }
    bool Map::getTileXY( int scrx, int scry, int& tilex, int& tiley )
    {
        if ( ( scrx < rect.x || scrx > rect.x + rect.w )
            || ( scry < rect.y || scry > rect.y + rect.h ) )
            return false;
     
        const int scrTW = tilew/m_scale;
        const int scrTH = tileh/m_scale;
        tilex =  ( view.x/m_scale + scrx - rect.x )/ scrTW;
        tiley =  ( view.y/m_scale + scry - rect.y )/ scrTH;

        return true;
    }
//====================================================================================================//


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

    Font::Font()
         :start(0), charW(0), charH(0)
    {

    }

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
            //handle newlines
            if(s == '\n' || s == KEY_RETURN)
            {
                i++;
                if(i== text.size()) break;
                scrolly++;
                s =  text[i];
                dx = 0;
                dy++;
            }
            else if(s == KEY_TAB)
            {
                dx += TAB_SIZE;
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
    bool Font::deserialize( std::istream& in )
    {
        try 
        {   
            std::getline( in, this->name, '\n' ); 
            FS::ReplaceAll(this->name, "\r", "");
            int w,h, blocksize;
            int startInt;
            in >> w >> h >> charW >> charH >> startInt >> blocksize ;
            //TODO - verify endian-ness!
            char * imagedata = new char[blocksize]; 
            in.read(imagedata, blocksize);

            start= startInt;
            reset((Color*)imagedata, w,h);

        }
        catch (...)
        {
            LOG("Assets: Failed to load font %s\n", name.c_str());
            return 0;
        }
        return 1;
    }   

    void Font::serialize(std::ostream & out) const 
    {
        out << name << std::endl;
        //TODO - verify endian-ness!
        const int blocksize = sizeof(Color) * w * h;
        
        out << w            << ' ' << h         << ' '; 
        out << charW        << ' ' << charH     << ' ' ; 
        out << (int)(start) << ' ' << blocksize ;
        
        out.write((char*)pixels, blocksize);
        LOG("Assets: Saved font\n");
    }


    ///////////////////////////////////////////////////////////////////////////////
    
    TextBox::TextBox(int tw, int th, const std::string & text, const std::string & font)
        :text(text),
        font(font),
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
        Assets::Unload<Font>(font);
        Engine::DestroyTexture(m_texture);
    }

    void TextBox::draw()
    {
        if (!visible) return;
        if(!m_fontcache) return;   
        Engine::DrawTexture(m_texture, {0,0,w,h}, {x,y,w,h});
    }
    const Font *TextBox::getFont() 
    { 
        return m_fontcache; 
    }

    void TextBox::refresh()
    {
        Engine::ClearTexture(m_texture, fillColor);
        m_fontcache->blit(m_texture, text, { borderX, borderY , w, h });
        for(int y =0 ; y < m_fontcache->h; y++ )
        {   
            for(int x =0 ; x < m_fontcache->w; x++ )
            {
                //color only text 
                Color & color = m_fontcache->pixels[y * m_fontcache->w + x];
                if(color == fillColor)
                    color = textColor;
            }
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