#pragma once
#include <typeinfo>
#include <string>

// Impl
namespace Graphics 
{
	class Sheet;
	class Sprite;
}
/*
	All assets must define a constructor with const std::string & name as a param and as a const, public, attrib. 
	
	name is guid
*/
class Asset
{
public:
	Asset(const std::string & name) ;
	virtual ~Asset() = default;
	const std::string name;
	int refcounter = 0;
};


namespace Assets
{
	void Startup(const std::string & assetdir);
	void Shutdown();

	//update the cache. Do not call each frame.
	//void Prune();
	
	Asset* LoadImpl(const std::type_info& type, const std::string& name);
	void SaveImpl(const Asset* asset, const std::type_info& type, const std::string& name);

	void Unload(const std::string& name);

	template <typename Type>
	Type * Load(const std::string& name) 
	{
		return dynamic_cast<Type*>(LoadImpl(typeid(Type), name));
	}
	
	template <typename Type>
	void Save(Type * asset, const std::string& name) 
	{
		return SaveImpl(asset, typeid(Type), name);
	}

}