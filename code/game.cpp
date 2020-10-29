#include "game.hpp"
#include "assets/api.hpp"
#include "../scripting/api.hpp"

namespace 
{

	bool s_mapsEnabled[LAYER_COUNT];
	Game::Cartridge * m_cart;
}


//use MMU 


namespace Game
{
	
	Header::Header( const std::string &name )
		:Asset(name)
	{}

	Cartridge::Cartridge( const Header & header, const Chunk & chunk )
		: header(header), chunk(chunk)
	{
	}


	Cartridge::~Cartridge()
	{
	}	
	
	Asset* Cartridge::LoadImpl( const std::type_info &type, const std::string &name )
	{
		Factory *factory = nullptr;
		
		if ( type == typeid( Graphics::Map ) )
		{
			factory = new MapFactory();
		}
		else if ( type == typeid( Graphics::Tileset ) )
		{
			factory = new TilesetFactory();
		}
		else if ( type == typeid( Script ) )
		{
			factory = new ScriptFactory();
		}

		const uint offset = chunk.offsets[name];
		std::istringstream iss;
		iss.str(chunk.data+offset);
		Asset* asset = factory->deserialize(iss);
		delete factory;
		return asset ;
	}

	void Cartridge::UnloadImpl( const std::type_info &type, Asset*  asset )
	{
		delete asset;
	}


///////////////////////////////////////////////////////////////////////////////

	void Startup(const std::string & cartpath)
	{
		//load cartridge header and data.
		m_cart = 0;
	}


	void Shutdown()
	{
		//unload cartrigde 

	}
	
	//writes asset meta data (name and offset in chunk)
	template <class Type, class FactoryType>
	static void PackAssetList( const std::vector<std::string>& names, std::unordered_map<std::string, int> &offsetmap, std::ostream & out )
	{
		FactoryType * factory = new FactoryType();
		for ( int i = 0; i < names.size(); i++ )
		{
			const std::string &name = names[i];
			if ( name.size() == 0 ) 
			{
				continue;
			}
			Type *asset = Assets::Load<Type>( name );
			ASSERT(asset, "CartridgeFactory: Could not find %s! ", name.c_str() );
			offsetmap[name] = out.tellp();
			factory->serialize( asset, out );
		}
		delete factory;
	}

	static void PackHeader( const std::vector<std::string>& names, const std::unordered_map<std::string, int> &offsetmap, std::ostream & out )
	{
		out << names.size() << "\n";\
		for ( int i = 0; i < names.size(); i++ )\
		{
			const std::string &name = names[i]; 
			if ( name.size() == 0 )
			{
				continue;
			}
			auto it = offsetmap.find( name );
			if ( it != offsetmap.end() )
			{
				out << it->second << "\n" << name << "\n";\
			}
		}
	}



	//package game into cartridge
	void Package( const Game::Header & header, const std::string &cartpath )
	{
		//push relative to header

		const std::string & fullpath = FS::FullPath( cartpath );
		const std::string & name = FS::BaseName( cartpath );
		std::ofstream out;
		out.open( fullpath );
		
		if ( !out.is_open() )
		{
			ASSERT( 0, "Failed to find cartridge %s", fullpath.c_str() );
			return;
		}
		Assets::PushPath( FS::DirName(cartpath) );

		std::ostringstream oss;
		Game::Chunk outChunk;
		//write out asset chunk to string stream
		PackAssetList<Graphics::Font, TilesetFactory>( header.fonts, outChunk.offsets, oss );
		PackAssetList<Graphics::Tileset, TilesetFactory>( header.tilesets, outChunk.offsets, oss );
		PackAssetList<Graphics::Map,     MapFactory    >( header.maps,     outChunk.offsets, oss );
		PackAssetList<Script,            ScriptFactory >( header.scripts,  outChunk.offsets, oss );
		outChunk.size = oss.tellp();
		if ( outChunk.size  > 0)
		{
			outChunk.data = new char[outChunk.size];
			strcpy_s( outChunk.data, outChunk.size, oss.str().c_str() );
		}
		//write header metadata.
		// num
		// offset name
		// offset name
		// offset name
		// chunk
		out << header.name << "\n";
		PackHeader( header.fonts , outChunk.offsets, out );
		PackHeader( header.tilesets, outChunk.offsets, out );
		PackHeader( header.maps,     outChunk.offsets, out );
		PackHeader( header.scripts,  outChunk.offsets, out );

		out << outChunk.size;
		out.write( outChunk.data, outChunk.size);

		out.close();
		Assets::PopPath( );
			
	}

	static void UnpackHeader( std::vector<std::string> &names, std::unordered_map<std::string, int> &offsetmap, std::istream &in )
	{
		int len ;
		int offset;
		std::string assetname;
		in >> len;
		names.resize( len );
		while ( len-- > 0 )
		{
			in >> offset; 
			std::getline( in, assetname );
			names.push_back( assetname);
			offsetmap[assetname] = offset;
		}
	}
	

	//package game into cartridge
	Game::Cartridge * Unpackage( const std::string &cartpath )
	{

		const std::string fullpath = FS::FullPath( cartpath );
		std::ifstream in;
		in.open( fullpath );
		
		Game::Cartridge * cartridge= nullptr;
		try
		{        
			if ( !in.is_open() )
			{
				throw;
			}
			std::string name;
			std::getline( in, name );

			Game::Header header(name);
			Game::Chunk chunk;
			
			UnpackHeader( header.fonts, chunk.offsets, in );
			UnpackHeader( header.tilesets, chunk.offsets, in );
			UnpackHeader( header.maps,     chunk.offsets, in );
			UnpackHeader( header.scripts,  chunk.offsets, in );
        
			//read chunk
			const int pos = in.tellg();
       		in.seekg (0, in.end);

				
			in >> chunk.size;
			chunk.data = new char[chunk.size];
			in.read( chunk.data, chunk.size );

			cartridge = new Game::Cartridge( header, chunk );
			
		}
		catch ( ... )
		{
			LOG( "Failed to find cartridge %s", fullpath.c_str());
		}
		in.close();
		return cartridge;
	}


}