#pragma once

#include <typeinfo>
#include <string>


//TODO Movet his to asset module. Graphics should not subclass Asset but rather create FontAsset, GameAsset etc. 
/*
	All assets must define a constructor with const std::string & name as a param and as a const, public, attrib. 
	
	name is guid
*/
class Asset
{
public:
	Asset(const std::string & name = "NO_NAME") ;
	virtual ~Asset() = default;

	virtual bool deserialize( std::istream& in ) = 0 ;
	virtual void serialize( std::ostream& out ) const = 0;

	std::string name;
	std::string filepath;
	int refcounter = 0;

};



namespace Assets
{
	void Startup(const std::string & dirpath);
	void Shutdown();
	//Run for the given game 
	void AddPath(const std::string & dirpath);
	//clear all but default path
	void ClearPaths();
	//update the cache. Do not call each frame.
	//void Prune();

	Asset* LoadImpl(const std::type_info& type, const std::string& name);
	void UnloadImpl(const std::type_info& type, const std::string& name);
	void SaveImpl( Asset* asset, const std::type_info& type, const std::string& name);
	void SaveAsImpl( Asset* asset, const std::type_info& type, const std::string& path);
	std::string GetAssetTypeExtImpl(const std::type_info& type);

	template <typename Type>
	void Unload(const std::string& name) 
	{
		UnloadImpl(typeid(Type), name);
	}

	template <typename Type>
	Type * Load(const std::string& name) 
	{
		return dynamic_cast<Type*>(LoadImpl(typeid(Type), name));
	}
	
	template <typename Type>
	void Save(Type * asset) 
	{
		return SaveImpl(asset, typeid(Type), asset->name);
	}
		
	template <typename Type>
	void SaveAs(Type * asset, const std::string& path ) 
	{
		return SaveAsImpl(asset, typeid(Type), path);
	}

	template <typename Type>
	std::string GetAssetTypeExt() 
	{
		return GetAssetTypeExtImpl(typeid(Type));
	}

}