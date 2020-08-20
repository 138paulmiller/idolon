#pragma once
#include "core.hpp"
//Create virtual drives. Map sys/ to systems dir and ~/ to home
namespace FS
{
	void ReplaceAll(std::string& str, const std::string& from, const std::string& to) ;
	//file to remove
	bool Move(const std::string& path, const std::string& newPath);
	bool Remove(const std::string& path);
	void Ls( std::vector<std::string>& files);

	//list files at the given dir 
	void Ls(const std::string& path, std::vector<std::string> &files);
	//path to exe
	std::string ExePath();	
	//get fullpath of file
	std::string FullPath(const std::string & filename);
	//return the virtual "root" path for the system
	std::string Root();
	std::string Cwd();
	bool  Cd(const std::string path);

	bool IsDir(const std::string & path);
	bool MkDir(const std::string & path);

	std::string Append(const std::string & path1, const std::string & path2);
	//filename without extension or preceding path
	std::string BaseName(const std::string& path);
	//remot base path without ext
	std::string NoExt( const std::string &path );
	//only returns file extension (ignoring .
	std::string FileExt(const std::string& path);
	//returns path to file (excluding filename )
	std::string DirName(const std::string& path);
}
