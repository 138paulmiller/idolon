#include "factories.hpp"

#include "../sys.hpp"


TilesetFactory::TilesetFactory()
:Factory( typeid(Graphics::Tileset), DEFAULT_TILESET_EXT )
{
}

Asset * TilesetFactory::deserialize(std::istream & in)
{
    Graphics::Tileset * tileset = 0;
    try 
    {
        std::string name;

        std::getline( in, name, '\n' ); 
        FS::ReplaceAll(name, "\r", "");
        int w, h;
        in >> w >> h;
        int blocksize;
        in >> blocksize ;
        char * imagedata = new char[blocksize]; 
        in.read(imagedata, blocksize);

        tileset = new Graphics::Tileset(name, w, h);
        tileset->reset( (Color*)imagedata, w , h);
    }
    catch (...)
    {
        if ( tileset )
        {
            LOG("TilesetFactory: Failed to load map %s\n", tileset->filepath.c_str());
            delete tileset;
            tileset = 0;
        }
        else 
        { 
            LOG( "TilesetFactory: Failed to load map\n" );
        }
    }
    return tileset;
}
    
void TilesetFactory::serialize(const Asset * asset, std::ostream & out) const 
{
    const Graphics::Tileset * tileset = dynamic_cast< const Graphics::Tileset* >( asset );
    ASSERT( tileset , "TilesetFactory: Asset not loaded" );
    out << tileset->name << std::endl;
    //TODO - verify endian-ness!
    int blocksize = sizeof(Color) * tileset->w * tileset->h;
    out << tileset->w << ' ' << tileset->h << ' ' << blocksize;
    out.write((char*)tileset->pixels, blocksize);
    LOG("TilesetFactory: Saved %s\n",  tileset->name.c_str());
}

///////////////////////////////////////////////////////////////////////////////

MapFactory::MapFactory()
:Factory( typeid(Graphics::Map), DEFAULT_MAP_EXT )
{
}


Asset *  MapFactory::deserialize( std::istream & in)
{
    Graphics::Map * map = 0;

    try 
    {
        std::string name;
        std::getline( in, name, '\n' ); 
        FS::ReplaceAll(name, "\r", "");
        std::string tilesets[TILESETS_PER_MAP];
        for ( int i = 0; i < TILESETS_PER_MAP; i++ )
        {
            std::getline( in, tilesets[i], '\n' ); 
            FS::ReplaceAll(tilesets[i], "\r", "");
        }
        int w, h, tilew, tileh;
        in >> w >> h >> tilew >> tileh;

        //TODO - verify endian-ness!
        int blocksize;
        in >> blocksize ;
        char * tiledata = new char[blocksize]; 
        in.read(tiledata, blocksize);
     
        map = new Graphics::Map( name );
        //tilesets
        for ( int i = 0; i < TILESETS_PER_MAP; i++ )
        {
            map->tilesets[i] = tilesets[i];
        }
        map->reset(tiledata, w, h, tilew, tileh);

        LOG("Factory: Loaded map %s\n", map->name.c_str());

    }
    catch (...)
    {
        if ( map )
        {
            LOG("MapFactory: Failed to load map %s\n", map->filepath.c_str());
            delete map;
            map= 0;
        }
        else 
        {
            LOG("MapFactory: Failed to load map\n");
        }
    }
    return map;
}   
    
void MapFactory::serialize(const Asset * asset, std::ostream & out) const
{
    const Graphics::Map * map = dynamic_cast< const Graphics::Map* >( asset );

    ASSERT( map , "MapFactory: Asset not loaded" );

    out << map->name << std::endl;
    for(int i = 0 ; i < TILESETS_PER_MAP; i++ )
    {
        out << map->tilesets[i] << std::endl;
    }
    //TODO - verify endian-ness!
    int blocksize =  map->w * map->h;
    out << map->w << ' ' << map->h << ' ';
    out << map->tilew << ' ' << map->tileh << ' ';
    out << blocksize;
    out.write((char * )map->tiles, blocksize);
    LOG("MapFactory: Saved %s\n",  map->name.c_str());
}

/////////////////////////////////////////////////////////////////////////////////
FontFactory::FontFactory()
:Factory( typeid(Graphics::Font), DEFAULT_FONT_EXT )
{
}

Asset *  FontFactory::deserialize( std::istream& in )
{
    Graphics::Font *font = 0;
    try 
    {
        std::string name;
        std::getline( in, name, '\n' ); 
        FS::ReplaceAll(name, "\r", "");
        int w,h, charW, charH, blocksize;
        int start;
        in >> w >> h >> charW >> charH >> start >> blocksize ;
        //TODO - verify endian-ness!
        char * imagedata = new char[blocksize]; 
        in.read(imagedata, blocksize);

        font = new Graphics::Font( name, w, h, charW, charH, start );
        font->reset((Color*)imagedata, w,h);

    }
    catch (...)
    {
        if ( font )
        {
            LOG("FontFactory: Failed to load font %s\n",font->filepath.c_str());
            delete font;
            font = 0;
        }
        else
        {
            LOG("FontFactory: Failed to load font\n");
        }
    }
    return font;
}   

void FontFactory::serialize(const Asset * asset, std::ostream & out) const 
{
    const Graphics::Font* font = dynamic_cast< const Graphics::Font* >( asset );

    ASSERT( font , "FontFactory: Asset not loaded" );
    out << font->name << std::endl;
    //TODO - verify endian-ness!
    const int blocksize = sizeof(Color) * font->w * font->h;
        
    out << font->w            << ' ' << font->h         << ' '; 
    out << font->charW        << ' ' << font->charH     << ' ' ; 
    out << (int)font->start   << ' ' << blocksize ;
        
    out.write((char*)font->pixels, blocksize);
    LOG("FontFactory: Saved %s\n",  font->name.c_str());
}

//////////////////////////////////////////////////////////////////////////



ScriptFactory::ScriptFactory()
:Factory( typeid(Script), DEFAULT_SCRIPT_EXT )
{
}

Asset * ScriptFactory::deserialize( std::istream& in )
{

    static const std::unordered_map<std::string, ScriptLanguage > LangStrToEnum = 
    {
	    { "none", SCRIPT_NONE },
	    { "python", SCRIPT_PYTHON }
    };

    Script *script = 0;
	try
	{
		//
        std::string name;
		std::string header;

		//get the code portion
		in.seekg (0, in.end);
        const int len = in.tellg();
        in.seekg ( 0, in.beg );

        std::getline( in, header ); 

        const int codelen = len - header.size();

        FS::ReplaceAll(header, "\r", "");
		
		//expect $ <name> <lang>
		if ( header.size() < 5 && header[0] == '$'  ) //minimal possible "$ a p"
		{
			LOG( "ScriptFactory: Corrupted header %s\n", header.c_str() );
			throw;
		}
		else
		{
			//parse header
            header = Trim(header.substr( 1 ));
			int off = 0;
			name = header.substr( off, header.find_first_of(' ' ) );
			off += name.size();
			std::string langstr = Trim(header.substr( off ));
			name = Trim(name);

            auto it = LangStrToEnum.find( langstr );
			if ( it == LangStrToEnum.end() )
			{
				LOG( "ScriptFactory: Invalid language %s\n", langstr.c_str() );
				throw;
			}
			ScriptLanguage lang = it->second;
		
			char * codedata = new char[codelen]; 
            memset(codedata, 0, codelen);
			in.read(codedata, len);
			
            switch ( lang )
            {
            case SCRIPT_PYTHON:
                script  = new PyScript( name );
                break;
            }
            script->lang = lang;
			script->code = std::string(codedata);
			
            delete[] codedata;

		}
	}
	catch(...)
	{
        if ( script )
        {
		    LOG("Script : failed to load script %s\n", script->filepath.c_str());
            delete script ;
            script = 0;
        }
        else
        {
            LOG("Script : failed to load script\n");
        }
    }
    return script ;
}

void ScriptFactory::serialize( const Asset * asset, std::ostream& out ) const
{
    static const std::string LangEnumToStr[] = 
    {
	    "none",
	    "python"
    };

    const Script* script = dynamic_cast< const Script* >( asset );
    ASSERT( script , "ScriptFactory: Asset not loaded" );

    out << "$" << script->name << " " << LangEnumToStr[script->lang] << std::endl;
	out << script->code;
}

//////////////////////////////////////////////////////////////////////////////////////////////
GameDescFactory::GameDescFactory()
:Factory( typeid(Game::Desc), DEFAULT_GAMEDESC_EXT )
{
}
Asset *GameDescFactory::deserialize( std::istream &in )
{
    Game::Desc *desc= 0;
    try
    {        
        std::string name;
        std::getline( in, name );

        desc = new Game::Desc(name);
        std::string subassetName;
        
        int len = 0;
        in >> len; 
        desc->tilesets.resize( len );
        while ( len-- > 0 )
        {
            std::getline( in, subassetName );
            desc->tilesets.push_back( subassetName );
        }

        in >> len; 
        desc->maps.resize( len );
        while ( len-- > 0 )
        {
            std::getline( in, subassetName );
            desc->maps.push_back( subassetName);
        }
        
        in >> len; 
        desc->scripts.resize( len );
        while ( len-- > 0 )
        {
            std::getline( in, subassetName );
            desc->scripts.push_back( subassetName);
        }

    }
    catch ( ... )
    {
        if ( desc )
        {
		    LOG("Game Desc : failed to load game description %s\n", desc->filepath.c_str());
            delete desc ;
            desc = 0;
        }
        else
        {
            LOG("Game Desc : failed to load game description\n");
        }
    }
    return desc;
}

void GameDescFactory::serialize( const  Asset *asset, std::ostream &out ) const
{
}
