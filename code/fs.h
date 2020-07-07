#pragma once
#include "core.h"

namespace FS
{
	//file to remove
	bool Move(const std::string& path, const std::string& newPath);
	bool Remove(const std::string& path);
	//list files at the given dir 
	void List(const std::string& path, std::vector<std::string> &files);
	//path to exe
	std::string ExePath();	
	//get fullpath of file
	std::string FullPath(const std::string & filename);
	//return the virtual "root" path for the system
	std::string Root();

	bool IsDir(const std::string & path);
	bool MkDir(const std::string & path);

	std::string Append(const std::string & path1, const std::string & path2);

	//filename without extension or preceding path
	std::string BaseName(std::string path);
	//only returns file extension (ignoring .
	std::string FileExt(const std::string& path);
	//returns path to file (excluding filename )
	std::string DirName(const std::string& path);
}
