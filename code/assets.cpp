#include "assets.h"
#include "graphics.h"
//#include "scripts.h"
#include <fstream>
#include <iostream>


//TODO replace fstream with C file
namespace
{
	template<class Type>
	using Table = std::unordered_map<std::string, Type>;
	Table<Asset*> s_assets;	

	std::string s_assetdir; 

}

Asset::Asset(const std::string& name) :name(name)
{}
	

namespace Assets
{
	std::string GetAssetTypeExt(const std::type_info& type)
	{

		if(type ==  typeid(Graphics::Sprite))
			return ".sprite";
		if(type ==  typeid(Graphics::Sheet))
			return ".sheet";
		if(type ==  typeid(Graphics::Font))
			return ".font";
	}


	std::string MakeAssetPath(const std::type_info& type, const std::string & name)
	{
		return s_assetdir + std::string("/") + name + GetAssetTypeExt(type);
	}

	void Startup(const std::string & assetdir)
	{
		s_assetdir = assetdir;

	}

	void Shutdown()
	{}

	
	Graphics::Sheet* LoadSheet(const std::string & name, const std::string & path)
	{
		Graphics::Sheet* sheet =0;
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

			sheet = new Graphics::Sheet(name, w, h);
			//uncompress bytes into pixels data
			memcpy(sheet->pixels, imagedata, sizeof(Color) * w * h);
			sheet->update();
			delete[] imagedata;

		}
		catch (...)
		{
			if(sheet)
				delete sheet;
				sheet = 0;
			printf("Asset:Failed to load %s\n", name.c_str());
		}
		return sheet;
	}
	void SaveSheet(const Graphics::Sheet* sheet, const std::string & path)
	{
		try 
		{		
			std::fstream outfile;
			outfile.open(path, std::fstream::out);
			outfile << sheet->name << std::endl;
			//TODO - verify endian-ness!
			int blocksize = sizeof(Color) * sheet->w * sheet->h;
			outfile << sheet->w << ' ' << sheet->h << ' ' << blocksize;
			outfile.write((char*)sheet->pixels, blocksize);
			outfile.close();

		}
		catch (...)
		{
			if(sheet)
				delete sheet;
				sheet = 0;
			printf("Assets: Failed to save %s", sheet->name.c_str());
		}
	}
	
	Graphics::Sprite* LoadSprite(const std::string & name, const std::string & path)
	{
		assert(0);
		return 0;
	}

	void SaveSprite(const Graphics::Sprite* sprite, const std::string & path)
	{
		assert(0);
	}

	Graphics::Font* LoadFont(const std::string & name, const std::string & path)
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
			printf("Asset:Failed to load %s\n", path.c_str());
		}
		printf("Asset:Loaded %s\n", path.c_str());
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
			int blocksize = sizeof(Color) * font->w * font->h;
			outfile << font->w << ' ' << font->h << ' '; 
			outfile << font->charW << ' ' << font->charH  << ' ' ; 
			outfile << (int)(font->start) << ' ';
			outfile << blocksize;
			outfile.write((char*)font->pixels, blocksize);
			outfile.close();

		}
		catch (...)
		{
			if(font)
				delete font;
				font = 0;
			printf("Assets: Failed to save %s", font->name.c_str());
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
		printf("Assets: Failed to unload (%s)\n", name.c_str());
	}

	Asset* LoadImpl(const std::type_info& type, const std::string& name)
	{	
		const std::string & path = MakeAssetPath(type, name);

		Table<Asset*>::iterator it = s_assets.find(name);		
		if (it != s_assets.end()) 									
		{						
			printf("Assets: Retrieved %s\n", path.c_str());

			Asset * asset = it->second;					
			asset->refcounter += 1;								
			return asset;									
		}			
		//asset to filename 
		printf("Assets: Loading %s\n", path.c_str());

		if(type ==  typeid(Graphics::Sprite))
			return s_assets[name] = LoadSprite(name, path );
		if(type ==  typeid(Graphics::Sheet))
			return s_assets[name] = LoadSheet(name, path );
		if(type ==  typeid(Graphics::Font))
			return s_assets[name] = LoadFont(name, path );
		return 0;
	}

	void SaveImpl(const Asset* asset, const std::type_info& type, const std::string& name)
	{
		//asset to filename 
		const std::string & path = MakeAssetPath(type, name);
		printf("Assets: Saving %s\n",path.c_str());

		if(type ==  typeid(Graphics::Sprite))
			SaveSprite(dynamic_cast<const Graphics::Sprite*>(asset),  path );
		else if(type ==  typeid(Graphics::Sheet))
			SaveSheet(dynamic_cast<const Graphics::Sheet*>(asset), path );	
		else if(type ==  typeid(Graphics::Font))
			SaveFont(dynamic_cast<const Graphics::Font*>(asset), path );	
	}




}