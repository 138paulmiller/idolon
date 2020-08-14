
#include "fs.hpp"
#include "assets.hpp"
#include "graphics.hpp"

#include <fstream>
#include <iostream>


/*TODO 
	replace fstream with C file
	have save and load as virtual functions for asset type ? 

 */
namespace
{
	template<class Type>
	using Table = std::unordered_map<std::string, Type>;
	Table<Asset*> s_assets;	

	std::vector<std::string> s_assetdirs; 

} // namespace

Asset::Asset(const std::string& name) :name(name)
{
	
}
	

namespace Assets
{
	std::string GetAssetTypeExtImpl(const std::type_info& type)
	{
		if(type ==  typeid(Graphics::Map))
		{
			return ".map";
		}
		if(type ==  typeid(Graphics::Tileset))
		{
			return".tls";
		}	
		if(type ==  typeid(Graphics::Font))
		{
			return ".fnt";
		}
		return ".raw";
	}

	bool FileExists(const std::string & filepath)
	{
	    std::ifstream infile(filepath);
	    return infile.good();
	}

	std::string FindAssetPath(const std::type_info& type, const std::string & name)
	{
		std::string n = name;
		for(const std::string & dirpath : s_assetdirs)
		{
			const std::string & path = FS::Append(dirpath, n) + GetAssetTypeExtImpl(type);
			if(FileExists(path))
			{
				return path;
			}
		}
		return "";
	}

	void Startup(const std::string & dirpath)
	{
		s_assetdirs.push_back(dirpath);
	}

	void Shutdown()
	{
		for(auto & pair : s_assets)
		{
			delete pair.second;
		}
	}

	void AddPath(const std::string & dirpath)
	{
		s_assetdirs.push_back(dirpath);
	}

	void ClearPaths()
	{
		//default path
		std::string path = s_assetdirs[0];
		s_assetdirs.clear();
		s_assetdirs.push_back(path);   
	}
	/*
	        std::string sheet;
        //viewport
        Rect view;
        const int w,h, tw, th;
     private:
        int * m_tiles;
        Tileset * m_tilesetcache;
        //TODO - split map into multiple subtextures. Each streamed in on demand. "Super maps"
        const int texture; 

	*/
	void SaveMap(const Graphics::Map* map, const std::string & path)
	{
		try 
		{		
			std::fstream outfile;
			outfile.open(path, std::fstream::out);
			outfile << map->name << std::endl;
			for(int i = 0 ; i < TILESETS_PER_MAP; i++ )
			{
				outfile << map->tilesets[i] << std::endl;
			}
			//TODO - verify endian-ness!
			int blocksize =  map->w * map->h;
			outfile << map->w << ' ' << map->h << ' ';
			outfile << map->tilew << ' ' << map->tileh << ' ';
			outfile << blocksize;
			outfile.write((char * )map->tiles, blocksize);
			outfile.close();
			LOG("Assets: Saved map\n");

		}
		catch (...)
		{
			LOG("Assets: Failed to save %s\n", map->name.c_str());
		}
	}
	Graphics::Map* LoadMap( const std::string& path )
	{
		Graphics::Map* map =0;
		try 
		{
			std::fstream infile;
			infile.open(path, std::fstream::in);
			std::string name;
			std::string tilesets[TILESETS_PER_MAP];
			std::getline( infile, name, '\n' ); 
			FS::ReplaceAll(name, "\r", "");

			for ( int i = 0; i < TILESETS_PER_MAP; i++ )
			{
				std::getline( infile, tilesets[i], '\n' ); 
				FS::ReplaceAll(tilesets[i], "\r", "");

			}
			int w,h, tw, th;
			infile >> w >> h >> tw >> th;

			//TODO - verify endian-ness!
			int blocksize;
			infile >> blocksize ;
			char * tiledata = new char[blocksize]; 
			infile.read(tiledata, blocksize);
			infile.close();

			map = new Graphics::Map(name, w, h, tw, th);
			for ( int i = 0; i < TILESETS_PER_MAP; i++ )
			{
				map->tilesets[i] = tilesets[i];
			}
			//uncompress bytes into pixels data
			memcpy(map->tiles, tiledata, w * h);
			map->reload();
			delete[] tiledata;

		}
		catch (...)
		{
			if(map)
				delete map;
			map = 0;
			LOG("Asset:Failed to load %s\n", path.c_str());
		}
		return map;

	}
	Graphics::Tileset* LoadTileset(const std::string & path)
	{
		Graphics::Tileset* tileset =0;
		try 
		{
			std::fstream infile;
			infile.open(path, std::fstream::in);
			std::string name;
			std::getline( infile, name, '\n' ); 
			FS::ReplaceAll(name, "\r", "");

			int w,h;
			infile >> w >> h;

			//TODO - verify endian-ness!
			int blocksize;
			infile >> blocksize ;
			char * imagedata = new char[blocksize]; 
			infile.read(imagedata, blocksize);
			infile.close();

			tileset = new Graphics::Tileset(name, w, h);
			//uncompress bytes into pixels data
			memcpy(tileset->pixels, imagedata, sizeof(Color) * w * h);
			tileset->update();
			delete[] imagedata;

		}
		catch (...)
		{
			if(tileset)
				delete tileset;
				tileset = 0;
			LOG("Asset:Failed to load %s\n", path.c_str());
		}
		return tileset;
	}
	
	void SaveTileset(const Graphics::Tileset* tileset, const std::string & path)
	{
		try 
		{		
			std::fstream outfile;
			outfile.open(path, std::fstream::out);
			outfile << tileset->name << std::endl;
			//TODO - verify endian-ness!
			int blocksize = sizeof(Color) * tileset->w * tileset->h;
			outfile << tileset->w << ' ' << tileset->h << ' ' << blocksize;
			outfile.write((char*)tileset->pixels, blocksize);
			outfile.close();
			LOG("Assets: Saved tileset\n");

		}
		catch (...)
		{
			LOG("Assets: Failed to save %s\n", tileset->name.c_str());
		}
	}
	Graphics::Font* LoadFont(const std::string & path)
	{
		Graphics::Font* font =0;
		try 
		{
			std::fstream infile;
			infile.open(path, std::fstream::in);
			std::string name;
			std::getline( infile, name, '\n' ); 
			FS::ReplaceAll(name, "\r", "");

			int w,h, cw, ch, start, blocksize;
			infile >> w >> h >> cw >> ch >> start >> blocksize ;
			//TODO - verify endian-ness!
			char * imagedata = new char[blocksize]; 
			infile.read(imagedata, blocksize);
			infile.close();

			font = new Graphics::Font(name, w, h, cw, ch, start);
			
			//uncompress bytes into pixels data
			memcpy(font->pixels, imagedata, sizeof(Color) * w * h);
			font->update();
			delete[] imagedata;

		}
		catch (...)
		{
			if(font)
				delete font;
				font = 0;
			LOG("Assets: Failed to load %s\n", path.c_str());
		}
		LOG("Assets: Loaded %s\n", path.c_str());
		return font;
	}

	void SaveFont(const Graphics::Font* font, const std::string & path)
	{
		try 
		{		
			std::fstream outfile;
			outfile.open(path, std::fstream::out);
			outfile << font->name << std::endl;
			//TODO - verify endian-ness!
			const int blocksize = sizeof(Color) * font->w * font->h;
			
			outfile << font->w            << ' ' << font->h      << ' '; 
			outfile << font->charW        << ' ' << font->charH  << ' ' ; 
			outfile << (int)(font->start) << ' ';
			outfile << blocksize;
			
			outfile.write((char*)font->pixels, blocksize);
			outfile.close();

			LOG("Assets: Saved font\n");
		}
		catch (...)
		{
			LOG("Assets: Failed to save %s\n", font->name.c_str());
		}
	}
	// -------------------------- Impl -----------------------
	
	void UnloadImpl(const std::type_info& type, const std::string& name)
	{
		std::string path =  FindAssetPath(type, name);

		Table<Asset*>::iterator it = s_assets.find(path);		
		if (it != s_assets.end()) 									
		{			
			Asset * asset = it->second;	
			if(asset)
			{
				asset->refcounter -= 1;							
				if (asset->refcounter == 0)
				{
					delete asset;
					s_assets.erase(path);
					LOG("Assets: Unloaded %s\n", path.c_str());
				}
				return;
			}
		}	
	}

	Asset* LoadImpl(const std::type_info& type, const std::string& name)
	{	
		//LOG("Assets: loading %s\n", name.c_str());
		
		std::string path =  FindAssetPath(type, name);
		
		if(path.size() == 0)
		{
			LOG("Assets: Could not find %s\n", name.c_str());
			return 0;
		}

		Table<Asset*>::iterator it = s_assets.find(path);		
		if (it != s_assets.end()) 									
		{						
			//LOG("Assets: Retrieved %s\n", path.c_str());

			Asset * asset = it->second;					
			asset->refcounter += 1;								
			return asset;									
		}			
		//asset to filename 
		LOG("Assets: %s found at %s\n", name.c_str(), path.c_str());
		Asset * asset =0 ;
	
		if(type ==  typeid(Graphics::Map))
			asset= LoadMap(path );

		else if(type ==  typeid(Graphics::Tileset))
			asset = LoadTileset(path );
	
		else if(type ==  typeid(Graphics::Font))
			asset = LoadFont(path );
	
		s_assets[path] = asset;
		asset->refcounter += 1;								

		return asset;
	}
	void SaveAsImpl(const Asset* asset, const std::type_info& type, const std::string& path)
	{
		LOG("Assets: Saving %s\n",path.c_str());

		if(type ==  typeid(Graphics::Map))
			SaveMap(dynamic_cast<const Graphics::Map*>(asset),  path );
		
		else if(type ==  typeid(Graphics::Tileset))
			SaveTileset(dynamic_cast<const Graphics::Tileset*>(asset), path );	
		
		else if(type ==  typeid(Graphics::Font))
			SaveFont(dynamic_cast<const Graphics::Font*>(asset), path );		
	}
	void SaveImpl(const Asset* asset, const std::type_info& type, const std::string& name)
	{

		std::string n = name;
		//asset to filename 
		std::string path = FindAssetPath(type, name);
		if(path.size() == 0)
		{
			LOG("Assets: Failed to find file for asset (%s) falling back on %s\n", name.c_str(), asset->filepath.c_str());
			path = asset->filepath;
		}
		SaveAsImpl(asset, type, path);
	}

} // namespace Assets