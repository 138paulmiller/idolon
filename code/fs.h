#pragma once
#include "core.h"

namespace FS
{
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

}