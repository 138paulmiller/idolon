#include "pch.hpp"

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
{}
	

namespace Assets
{
	std::string GetAssetTypeExtImpl(const std::type_info& type)
	{
		if(type ==  typeid(Graphics::Map))
			return ".map";
		if(type ==  typeid(Graphics::Tileset))
			return ".tls";
		if(type ==  typeid(Graphics::Font))
			return ".fnt";
		return "";
	}

	bool FileExists(const std::string & filepath)
	{
	    std::ifstream infile(filepath);
	    return infile.good();
	}

	std::string FindAssetPath(const std::type_info& type, const std::string & name)
	{
		std::string path ;
		for(const std::string & dirpath : s_assetdirs)
		{
			path = FS::Append(dirpath, name) + GetAssetTypeExtImpl(type);
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
	void SaveMap(const Graphics::Map* map, const std::string & path)
	{
		ASSERT( 0 , "");
	}
	Graphics::Map* LoadMap( const std::string& path )
	{
		ASSERT( 0 , "");
		return 0;
	}
	Graphics::Tileset* LoadTileset(const std::string & path)
	{
		Graphics::Tileset* tileset =0;
		try 
		{
			std::fstream infile;
			infile.open(path, std::fstream::in);
			std::string name;
			std::getline( infile, name ); 
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
			LOG("Assets: Saved tileset");

		}
		catch (...)
		{
			LOG("Assets: Failed to save %s", tileset->name.c_str());
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
			std::getline( infile, name ); 
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

			LOG("Assets: Saved font");
		}
		catch (...)
		{
			LOG("Assets: Failed to save %s", font->name.c_str());
		}
	}
	// -------------------------- Impl -----------------------
	
	void Unload(const std::string& name)
	{
		Table<Asset*>::iterator it = s_assets.find(name);		
		if (it != s_assets.end()) 									
		{														
			Asset * asset = it->second;	
			if(asset)
			{
				asset->refcounter -= 1;							
				if (asset->refcounter == 0)
				{
					delete asset;
					s_assets.erase(name);
				}
				return;
			}
		}	
		LOG("Assets: Failed to unload (%s)\n", name.c_str());
	}

	Asset* LoadImpl(const std::type_info& type, const std::string& name)
	{	
		std::string path =  FindAssetPath(type, name);
		
		if(path.size() == 0)
		{
			LOG("Assets: %s Does not exist\n", name.c_str());
			return 0;
		}

		Table<Asset*>::iterator it = s_assets.find(name);		
		if (it != s_assets.end()) 									
		{						
			LOG("Assets: Retrieved %s\n", path.c_str());

			Asset * asset = it->second;					
			asset->refcounter += 1;								
			return asset;									
		}			
		//asset to filename 
		LOG("Assets: Loading %s\n", path.c_str());
		Asset * asset =0 ;
	
		if(type ==  typeid(Graphics::Map))
			asset= LoadMap(path );

		else if(type ==  typeid(Graphics::Tileset))
			asset = LoadTileset(path );
	
		else if(type ==  typeid(Graphics::Font))
			asset = LoadFont(path );
	
		s_assets[name] = asset;
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
		//asset to filename 
		std::string path = FindAssetPath(type, name);
		if(path.size() == 0)
		{
			LOG("Assets: Failed to find asset falling back on %s\n", asset->filepath.c_str());
			path = asset->filepath;
		}
		SaveAsImpl(asset, type, path);
	}

} // namespace Assets