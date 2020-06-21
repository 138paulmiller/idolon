#include "pch.h"
#include "assets.h"
#include "graphics.h"
//#include "scripts.h"
#include <fstream>
#include <json.hpp>
using namespace::tao;

namespace
{
	template<class Type>
	using Table = std::unordered_map<std::string, Type>;
	
	Table<Asset*> s_assets;	
}

Asset::Asset(const std::string& name) :name(name)
{}
	

namespace Assets
{
	const std::string& AssetPath(const std::string & name)
	{
		return "assets/" + name;
	}
	
	Graphics::Sheet* LoadSheet(const std::string & name, const std::string & path)
	{
		Graphics::Sheet* sheet =0;
		try 
		{
			auto object = json::from_file(path);
			;
			const std::string& name = object.as<std::string>("name");
			const int & w = object.as<int >("width");
			const int & h = object.as<int >("height");
			
			std::vector<std::byte> binary = object["binary"].get_binary();
			//4 bytes per color
			sheet = new Graphics::Sheet(name, w, h);
			memcpy(sheet->pixels, binary.data(), binary.size());
			sheet->update();
		}
		catch (...)
		{
			if(sheet)
				delete sheet;
				sheet = 0;
			printf("Asset:Failed to load  %s", name.c_str());
		}
		return sheet;
	}
	void SaveSheet(const Graphics::Sheet* sheet, const std::string & path)
	{

		try 
		{
			const int len = sheet->w * sheet->h * sizeof(Color);
			std::vector< std::byte > binary(len);
			memcpy(binary.data(), sheet->pixels, len );
			json::value object = {
				{ "name", sheet->name} ,
				{ "width", sheet->w },
				{ "height", sheet->h} ,
				{ "binary", binary   }
			};
			std::fstream outfile;
			outfile.open(path);
			json::to_stream(outfile, object);
			outfile.close();

		}
		catch (...)
		{
			if(sheet)
				delete sheet;
				sheet = 0;
			printf("Asset:Failed to save %s", sheet->name.c_str());
		}

	}
	
	Graphics::Sprite* LoadSprite(const std::string & name, const std::string & path)
	{
		return 0;
	}

	void SaveSprite(const Graphics::Sprite* sprite, const std::string & path)
	{
	}

	// -------------------------- Impl -----------------------
	
	void Unload(const std::string& name)
	{
		Table<Asset*>::iterator it = s_assets.find(name);		
		if (it != s_assets.end()) 									
		{														
			Asset * asset = it->second;					
			asset->refcounter -= 1;							
			if (asset->refcounter == 0)
			{
				delete asset;
				s_assets.erase(name);
			}
		}		
	}

	Asset* LoadImpl(const std::type_info& type, const std::string& name)
	{	
		Table<Asset*>::iterator it = s_assets.find(name);		
		if (it != s_assets.end()) 									
		{														
			Asset * asset = it->second;					
			asset->refcounter += 1;								
			return asset;									
		}			
		//asset to filename 
		const std::string & path = AssetPath(name);
		if(type ==  typeid(Graphics::Sprite))
			return s_assets[name] = LoadSprite(name, path );
		if(type ==  typeid(Graphics::Sheet))
			return s_assets[name] = LoadSheet(name, path );
		return 0;
	}

	void SaveImpl(const Asset* asset, const std::type_info& type, const std::string& name)
	{
		//asset to filename 
		const std::string & path = AssetPath(name);
		if(type ==  typeid(Graphics::Sprite))
			SaveSprite(dynamic_cast<const Graphics::Sprite*>(asset),  path );
		else if(type ==  typeid(Graphics::Sheet))
			SaveSheet(dynamic_cast<const Graphics::Sheet*>(asset), path );	
	}




}