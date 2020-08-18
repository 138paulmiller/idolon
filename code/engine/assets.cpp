
#include "api.hpp"
#include "../scripting/api.hpp"


/*TODO 
	replace fstream with C file
	have save and load as virtual functions for asset type ? 

 */

Asset::Asset(const std::string& name) 
	:name(name)
{
}

namespace
{
	template<class Type>
	using Table = std::unordered_map<std::string, Type>;
	Table<Asset*> s_assets;	

	std::vector<std::string> s_assetdirs; 

	struct AssetDetails
	{
		AssetDetails() = default;
		std::string ext;
		std::function<Asset*()> create;
	};
	std::unordered_map<const char*, AssetDetails> s_details ; 

} // namespace

	
namespace Assets
{
	std::string GetAssetTypeExtImpl(const std::type_info& type)
	{
		const auto it = s_details.find(type.name());
		if(it != s_details.end())
		{
			return s_details[type.name()].ext;
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
		LOG("Assets: Starting up...\n");
		s_assetdirs.push_back(dirpath);
		s_details = {
			{ 
				typeid(Graphics::Map).name(), 
				{ 
					".map", 
					[](){
						return new Graphics::Map(); 
					} 
				}
			},
			{ 
				typeid(Graphics::Tileset).name(), 
				{ 
					".tls", 
					[](){
						return new Graphics::Tileset(); 
					} 
				}
			},
			{ 
				typeid(Graphics::Font).name(), 
				{ 
					".fnt", 
					[](){
						return new Graphics::Font(); 
					} 
				}
			},
			{ 
				typeid(Script).name(), 
				{ 
					".scr", 
					[](){
						return new Script(); 
					} 
				}
			},
		};

	}

	void Shutdown()
	{
		s_details.clear();
		LOG("Assets:: Shutting down...\n");
		for(auto & pair : s_assets)
		{
			const Asset * asset = pair.second;
			if(asset->refcounter != 0)
			{
				LOG("Assets: Dangling reference to %s (%s)\n", asset->name.c_str(), asset->filepath.c_str() );
			}
			delete asset;
		}
		s_assets.clear();
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

	// -------------------------- Impl -----------------------
	Asset* DeserializeAsset(const std::type_info& type, std::istream& in )
	{
	
		Asset * asset = 0;

		const auto it = s_details.find(type.name());
		if(it == s_details.end())
			return 0;
		asset =  s_details[type.name()].create();

		if( !asset->deserialize(in) )
		{
			delete asset;
			asset = 0;
		}

		return asset;
	}

	void Serialize(const Asset * asset, std::ostream& out )
	{
		asset->serialize(out);
	}	
	
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
					LOG("Assets: Unloaded %s%s (%s)\n", asset->name.c_str(), GetAssetTypeExtImpl(type).c_str(), path.c_str());
				}
				return;
			}
		}	
	}

	Asset* LoadImpl(const std::type_info& type, const std::string& name)
	{			
		if ( name.size() == 0 ) return 0;
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
		LOG("Assets: Loading %s%s (%s)\n", name.c_str(), GetAssetTypeExtImpl(type).c_str(), path.c_str());
		Asset * asset =0 ;
	
		try 
		{
			std::fstream infile;
			infile.open(path, std::fstream::in);
			asset = DeserializeAsset(type, infile);
			infile.close();
		}
		catch (...)
		{
			if(asset)
				delete asset;
		}

		if(asset == 0)
		{
			LOG("Assets: Failed to load %s%s (%s)\n", name.c_str(), GetAssetTypeExtImpl(type).c_str(), path.c_str());
		}
		else
		{		
			s_assets[path] = asset;
			asset->refcounter += 1;			
			asset->filepath = path;
		}

		return asset;
	}
	void SaveAsImpl( Asset* asset, const std::type_info& type, const std::string& path)
	{
		if ( !asset ) return;
		asset->filepath = path;
		try 
		{		

			LOG("Assets: Saving %s%s (%s)\n", asset->name.c_str(), GetAssetTypeExtImpl(type).c_str(), path.c_str());

			std::ofstream outfile;
			outfile.open(path);
			Serialize(asset, outfile);
			outfile.close();
		}
		catch (...)
		{
			LOG("Assets: Failed to open %s for writing\n", path.c_str());
		}
	}
	void SaveImpl( Asset* asset, const std::type_info& type, const std::string& name)
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