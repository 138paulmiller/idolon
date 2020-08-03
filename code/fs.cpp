#include "pch.hpp"

#include "fs.hpp"
#include <algorithm>

#ifdef OS_WINDOWS

#include <windows.h>
#include <direct.h>

#define getcwd _getcwd
#define mkdir _mkdir
#define rmdir _rmdir
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
namespace 
{
	std::string s_cwd = "";
}


namespace FS
{
	void ReplaceAll(std::string& str, const std::string& from, const std::string& to) 
	{
	    if(from.empty())
	        return;
	    size_t pos = 0;
	    while((pos = str.find(from, pos )) != std::string::npos) 
		{
	        str.replace(pos , from.length(), to);
	        pos  += to.length(); 
	    }
	}
	bool CheckPath(const std::string & fullpath)
	{
		//do not navigate beyond root. compare substring to see if new dir is subdir of root
		//minus one, ignore the trailing / on root

		const std::string & root=FS::Root();
		const int n = root.size()-1;
		if(	(fullpath.size() < n ) || (root.compare(0, n, fullpath, 0, n) != 0))
			return 0;

		return 1;		
	}
	
	std::string Root()
	{
#ifdef OS_WINDOWS
		const char* mountPoint = "C:/Program Files/idolon/";
#elif defined(OS_LINUX)
		char mountPoint[MAX_PATH];
	    glob_t globbuf;
	 	glob("~", GLOB_TILDE, NULL, &globbuf);
        snprintf(mountPoint, sizeof(mountPoint), "%s/idolon/", globbuf.gl_pathv[0]);
#endif	
		if ( !FS::IsDir( mountPoint ) )
		{
			int status = mkdir(mountPoint);
			if ( status == -1 )
			{
				LOG( "Failed to create user space! Check app privileges" );
				exit( -1 );
			}
		}
		std::string root = mountPoint + std::string("root/") ;
		if ( !FS::IsDir( root ) )
		{
			int status = mkdir(root.c_str());
			if ( status == -1 )
			{
				LOG( "Failed to create user root! Check app privileges" );
				exit( -1 );
			}
		}
		return root;
	}
	std::string Cwd()
	{
		if (s_cwd.size() == 0)
			s_cwd = Root();
		return s_cwd;
	}
	
	bool Cd(const std::string path)
	{
		const std::string & newpath =  Append(Cwd(), path);
		if (CheckPath(newpath))
		{
			s_cwd = newpath;
			return true;
		}
		return false;
	}
	bool Move(const std::string& path, const std::string& newPath)
	{
		const std::string & a =  Append(Cwd(), path);
		const std::string & b =  Append(Cwd(), newPath);
		bool success = rename(a.c_str(), b.c_str()) != -1;
		return success;
	}

	bool Remove(const std::string& path)
	{
		const int errcode=
#ifdef OS_WINDOWS
			ENOENT;
#elif defined(OS_LINUX)
			-1;
#endif
		const std::string & fullpath = Append(Cwd(), path);
		if(!CheckPath(fullpath))
			return 0;

		bool status = 0;
		if (IsDir(fullpath))
		{
			std::vector<std::string> files;
			
			Ls(path, files);
			for (const std::string& file : files)
				if(file != ".." && !Remove(file)) return 0;

			status  = rmdir(fullpath.c_str() ) != errcode;
		}
		else
		{
			status = remove(fullpath.c_str()) == 0;		
		}
		return status;
	}
	void Ls(std::vector<std::string>& files)
	{
		Ls("", files);
	}
	void Ls(const std::string& path, std::vector<std::string>& files)
	{
		const std::string & fullpath = Append(Cwd(), path);
#ifdef OS_WINDOWS
        std::string pattern = fullpath  + "\\*";
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
					if (fullpath  == FS::Root() && filename == "..")
						continue;
					files.push_back(filename);
				}
			} while (FindNextFileA(hFind, &data));
            FindClose(hFind);
        }
#elif defined(OS_LINUX)
		DIR* dir = opendir(fullpath.c_str());
		if (dir) 
		{
			dirent * entry;
			while ((entry = readdir(dir))) 
			{
				std::string filename = entry->d_name;
				if (filename != "."  )
				{
					//
					//do not allow to navigate beyond root
					if (fullpath+"/"  == FS::Root() && filename == "..")
						continue;
					files.push_back(filename);
				}
			}
			closedir(dir);

		} else if (ENOENT == errno) 
		{
			LOG("FS: %s does not exist", fullpath.c_str());
		} else 
		{
		    /* opendir() failed for some other reason. */
			LOG("FS: %s failed to open", fullpath.c_str());
		}
#endif
	}

	std::string ExePath()
	{
		char buff[FILENAME_MAX]; 
		const char * s = getcwd(buff, FILENAME_MAX);
		std::string path = buff;
		std::replace( path.begin(), path.end(), '\\', '/'); 
		return path;
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
		if ( path.size() == 0 )return false;
		const std::string & fullpath = Append(Cwd(), path);

		//do not create dirs beyond root
		const std::string & root = FS::Root();
		if ( fullpath.compare(0, root.size(), root.c_str(), root.size()) != 0 )
				return false;
		return mkdir(fullpath.c_str()) != -1;	
	}


	std::string Append(const std::string& path1, const std::string& path2)
	{
		return FS::FullPath(path1 + "/" + path2);
	}

	std::string BaseName(const std::string& path)
	{
		std::string pathcpy = path;
		size_t sep = pathcpy.find_last_of("\\/");
		if (sep != std::string::npos)
			pathcpy = pathcpy.substr(sep + 1, pathcpy.size() - sep - 1);
		size_t dot = pathcpy.find_last_of(".");
		if (dot != std::string::npos)
		{
			return pathcpy.substr(0, dot);
		}
		return pathcpy;
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
} // namespace FS