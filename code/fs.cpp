#include "fs.h"

#ifdef OS_WINDOWS

#include <windows.h>
#include <direct.h>

#define getcwd _getcwd
#define mkdir _mkdir
#define chmod()

#elif defined(OS_LINUX)

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <glob.h>

#define mkdir(path) mkdir(path, 0777)

#define MAX_PATH 260

#endif


void ReplaceAll(std::string& str, const std::string& from, const std::string& to) 
{
    if(from.empty())
        return;
    int pos = 0;
    while((pos = str.find(from, pos )) != std::string::npos) 
	{
        str.replace(pos , from.length(), to);
        pos  += to.length(); 
    }
}

namespace FS
{
	void List(const std::string& path, std::vector<std::string>& files)
	{
#ifdef OS_WINDOWS
        std::string pattern = path + "\\*";
        HANDLE hFind;
		WIN32_FIND_DATAA data ;
        if ((hFind = FindFirstFileA(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) 
		{
			std::string filename;
			do {
				filename= data.cFileName;
				//ignore cur dir listing
				if (filename != "."  )
				{
					//do not allow to navigate beyond root
					if (path == FS::Root() && filename == "..")
						continue;
					files.push_back(filename);
				}
			} while (FindNextFileA(hFind, &data));
            FindClose(hFind);
        }
#elif defined(OS_LINUX)
		DIR* dir = opendir(path.c_str());
		dirent * entry;
		while ((entry = readdir(dir))) 
		{
			std::string filename = entry->d_name;
			if (filename != "."  )
			{
				//do not allow to navigate beyond root
				if (path == FS::Root() && filename == "..")
					continue;
				files.push_back(filename);
			}
		}
		closedir(dir);
#endif
	}

	std::string ExePath()
	{
		char buff[FILENAME_MAX]; 
		const char * s = getcwd(buff, FILENAME_MAX);
		return buff;
	}
	
	std::string FullPath(const std::string& filename)
	{
		char buff[MAX_PATH];
	
#ifdef OS_WINDOWS
		GetFullPathNameA(filename.c_str(), MAX_PATH, buff, nullptr);
	
#elif defined(OS_LINUX)
		realpath(filename.c_str(), buff);
#endif	
		std::string fullpath = buff;
		ReplaceAll(fullpath, "\\", "/");
		return fullpath;
	}

	std::string Root()
	{
#ifdef OS_WINDOWS
		const char* mountPoint = "C:/Program Files/UltBoy/root";
#elif defined(OS_LINUX)
		char mountPoint[MAX_PATH];
	    glob_t globbuf;
	 	glob("~", GLOB_TILDE, NULL, &globbuf);
        snprintf(mountPoint, sizeof(mountPoint), "%s/ultboy/root", globbuf.gl_pathv[0]);
#endif	
		int status = mkdir(mountPoint);
		return mountPoint;
	}

	bool IsDir(const std::string& path)
	{
#ifdef OS_WINDOWS
		DWORD attrib = GetFileAttributesA(path.c_str());
		//if not file and is dir
		return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(OS_LINUX)
		struct stat info;
		return (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
#endif
	}

	bool MkDir(const std::string& path)
	{
		//do not create dirs beyond root
		const std::string & root = FS::Root();
		if ( path.compare(0, root.size(), root.c_str(), root.size()) != 0 )
				return false;
		return mkdir(path.c_str()) != -1;	
	}


	std::string Append(const std::string& path1, const std::string& path2)
	{
		return FS::FullPath(path1 + "/" + path2);
	}

	std::string BaseName(std::string path)
	{
		size_t sep = path.find_last_of("\\/");
		if (sep != std::string::npos)
			path = path.substr(sep + 1, path.size() - sep - 1);
		size_t dot = path.find_last_of(".");
		if (dot != std::string::npos)
		{
			return path.substr(0, dot);
		}
		return path;
	}
	std::string FileExt(const std::string& path)
	{
		size_t dot = path.find_last_of(".");
		if (dot != std::string::npos)
		{
			return path.substr(dot+1);
		}
		return "";
	}

	std::string DirName(const std::string & path)
	{
		size_t sep = path.find_last_of("\\/");
		if (sep != std::string::npos)
			return path.substr(0, sep);
		return "";
	}
}