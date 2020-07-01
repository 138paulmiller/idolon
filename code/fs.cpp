#include "fs.h"

#ifdef OS_WINDOWS

#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#define mkdir _mkdir

#elif OS_LINUX

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

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
					files.push_back(data.cFileName);
				}
			} while (FindNextFileA(hFind, &data));
            FindClose(hFind);
        }
#elif OS_LINUX
		DIR* dir = opendir(path.c_str());
		dirent * entry;
		while ((entry = readdir(dir))) {
			files.push_back(entry->d_name);
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
	
#elif OS_LINUX
		asset(0);
#endif	
		std::string fullpath = buff;
		ReplaceAll(fullpath, "\\", "/");
		return fullpath;
	}

	std::string Root()
	{
#ifdef OS_WINDOWS
		const char * mountPoint= "C:/Program Files/UltBoy";
		mkdir(mountPoint);
		return mountPoint;
#elif OS_LINUX
		asset(0);
#endif	
	}

	bool IsDir(const std::string& path)
	{
#ifdef OS_WINDOWS
		DWORD attrib = GetFileAttributesA(path.c_str());
		//if not file and is dir
		return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#elif OS_LINUX
		struct stat info;
		return (stat(path, &info) && (info.st_mode & S_IFDIR));
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
}