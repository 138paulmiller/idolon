

#include "sys.hpp"

void PrintHelp(const Args& args);
void ImportAsset(const Args& args);
void NewAsset(const Args& args);
void EditAsset(const Args& args);


#define SHELL_LOG(...) \
{\
	const int n_  = 512;\
	char * msg_ = new char [n_];\
	snprintf(msg_, n_, __VA_ARGS__);\
	Sys::GetShell()->log(msg_);\
	delete[] msg_;\
}

//TODO - handle error more elegantly. Throw exception with msg ? Log mesage to shell ? with usage. 
#define ARG_NONEMPTY(args) if(args.size() <= 0  ) { SHELL_LOG("Missing arguments"); return; }
#define ARG_RANGE(args, min, max) if(args.size() < min || args.size() > max ) { SHELL_LOG("Incorrect argument range"); return; }
#define ARG_COUNT(args, i) if(args.size() != i ){ SHELL_LOG("Incorrect number of arguments"); return; }

static const CommandTable & g_cmds = 
{
	{ 	
		{ "help", "[action]" },
		PrintHelp
	},	
	{
		{ "exit", "shutdown system" },
		[](Args args)
		{ 
			Sys::Shutdown();
		} 
	},
	{
		{ "sys", "<cmd> <args>... system settings" }, 
		[](Args args)
		{ 
			ARG_NONEMPTY(args);
			if ( args[0] == "font" )
			{
				ARG_COUNT(args, 2);
				Sys::GetShell()->setFont(args[1]);
			}
		} 
	},
	{
		{ "import", "<asset> <name>"}, 
		ImportAsset
	},
	{
		{ "new", "<asset> <name> [args]"} ,
		NewAsset
	},
	{
		{ "edit", "<filename>"}, 
		EditAsset
	},
	{
		{ "ls", "[dir]+ : list content"}, 
		[](Args args)
		{ 
			std::vector<std::string> files;
			if(args.size() > 0)
			{
				for(const std::string & arg: args)
					FS::Ls(arg, files);
			}
			else
				FS::Ls(files);
			
			for (const std::string& file : files)
				SHELL_LOG("%s",file.c_str());
		} 
	},
	{
		{ "cd", "<dirname>"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::Cd(args[0]))
				SHELL_LOG("Directory does not exist");
		} 
	},
	{
		{ "cwd", ""}, 
		[](Args args)
		{ 
			SHELL_LOG("%s", FS::Cwd().c_str());
		} 
	},
	{
		{ "mkdir", "dir : create directory"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::MkDir(args[0]))
				SHELL_LOG("Could not create directory");
		} 
	},
	{
		{ "rm", "dir : remove directory"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::Remove(args[0]))
				SHELL_LOG("Failed to remove path");
		} 
	},
	{
		{ "mv", "path newpath : move to new path"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 2); // 
			
			if (!FS::Move(args[0], args[1]))
				SHELL_LOG("Failed to move");
		} 
	},
	//if running game, add all sub-directories as search dir for assets
	//on exit, clear the asset paths (will remove all but system)
};
	
static const CommandTable & g_cli = 
{
	{ 	
		{ "-cmd", "execute system commands " },
		[] ( Args args )
		{ 
			if ( args.size() <= 0 ) return;
			std::string command = "";
			while ( args.size() > 0 )
			{
				command = args.back() + " " +  command;
				args.pop_back();
			}
			Execute( command, g_cmds );
		}
	},	
	
};
//----------------------------


int try_main(int argc, char** argv)
{ 
	argc--; argv++;	//skip exe path arg

	Err::Stacktrace();

	Sys::Startup(g_cmds);

	Execute( argc, argv, g_cli );


	return Sys::Run();
}

int main(int argc, char** argv)
{ 
	try
	{
		return try_main(argc, argv);
	}
	catch (const std::exception &exc)
	{
	    printf("Caugth exception\n");
	    // catch anything thrown within try block that derives from std::exception
	    std::cerr << exc.what() << std::endl;
	}
	return -1;
}

////////////////// Commands  ///////////////////////////////////////


template <typename Type>
std::string SystemAssetPath(const std::string & name )
{
	return Sys::AssetPath() + (name + Assets::GetAssetTypeExt<Type>());
}

template <typename Type>
std::string UserAssetPath(const std::string & name )
{
	return FS::Cwd() + "/" + (name + Assets::GetAssetTypeExt<Type>());
}

void PrintHelp(const Args& args)
{
	std::string out;

	if(args.size()==1)
		Help(g_cmds, args[0], out);
	else 
		HelpAll(g_cmds, out);
	Sys::GetShell()->clear();
	SHELL_LOG("%s", out.c_str());
}

// -------------- Convert raw to asset ------------------------

#define ARG_NAME_TILESET "tls"
#define ARG_NAME_FONT "font"
#define ARG_NAME_MAP "map"


void ImportAsset(const Args& args)
{			
	ARG_NONEMPTY(args);
	const std::string &type = args[0];
	if(type == ARG_NAME_TILESET )
	{
		//must be two args
		ARG_COUNT(args, 2);
		int w, h;
		const std::string &  imgpath = args[1];
		Color * pixels = Engine::LoadTexture(imgpath, w, h);
		std::string name = FS::BaseName(imgpath);
		Graphics::Tileset * sheet = new Graphics::Tileset(name, w, h);
		memcpy(sheet->pixels, pixels, w * h * sizeof(Color));
		sheet->update();
		const std::string & filepath = SystemAssetPath<Graphics::Tileset>(name);
		Assets::SaveAs(sheet, filepath );

	}
	else if(type == ARG_NAME_FONT )
	{
		ARG_RANGE(args, 4, 6); // 
		const std::string &  imgpath = FS::Append(FS::Root(), args[1]);
		int cw = std::stoi(args[2]);
		int ch = std::stoi(args[3]);
		//charcter offset (ascii value)
		int start = args.size() > 4 ? std::stoi(args[4]) : 0;
		int w, h;
		Color * pixels = Engine::LoadTexture(imgpath, w, h);
		if ( !pixels ) return; //log err
		std::string name = FS::BaseName(imgpath);
		//remove spaces 
		int space = args.size() > 5 ? std::stoi(args[5]) : 0;
		int ci = w / ( cw + space );
		int cj = h / ( ch + space );
		int fw = cw * ci ;
		int fh = ch * cj;
		Color* newpixels = new Color[fw * fh * sizeof( Color )];
		for ( int j = 0; j <= cj; j++)
		{
			for ( int i = 0; i <= ci; i++ )
			{
				const int fx = i * cw;
				const int fy = j * ch;
				const int x =  space/2 + i * (cw+space);
				const int y =  space/2 + j * (ch+space);
				for(int dx = 0; dx<cw; dx++ )
				for(int dy = 0; dy<ch; dy++ )
					newpixels[(fy+dy) * fw + (fx+dx)] = pixels[(y+dy) * w + (x+dx)];
			}
		}
		Graphics::Font* font = new Graphics::Font( name, fw, fh, cw, ch, start );
		memcpy(font->pixels, newpixels, fw * fh * sizeof(Color));
		font->update();
		const std::string & filepath = SystemAssetPath<Graphics::Font>(name);
		Assets::SaveAs(font, filepath );

		delete[] newpixels;
	}
	else
	{
		SHELL_LOG("Invalid asset type (%s)", type.c_str());
	}
}


void NewAsset(const Args& args)
{			 
	//new <asset>  <name>  
	ARG_RANGE(args, 2, 3); 
	const std::string& type = args[0];
	const std::string& name = args[1];
	if(type == ARG_NAME_TILESET)
	{
		Graphics::Tileset * sheet = new Graphics::Tileset(name);
		Assets::SaveAs(sheet, UserAssetPath<Graphics::Tileset>(name));
	}
	else if(type == ARG_NAME_MAP)
	{
		Graphics::Map* map= new Graphics::Map(name);
		Assets::SaveAs(map, UserAssetPath<Graphics::Map>(name));
	}
	else if(type == ARG_NAME_FONT)
	{
		//8x8 chars. 18 x 6  
		const int w = 18 * 8;
		const int h =  6 * 8;
		Graphics::Font* font= new Graphics::Font(name, w, h);
		Assets::SaveAs(font, UserAssetPath<Graphics::Font>(name));
	}
	else
	{
		SHELL_LOG("Invalid asset type (%s)", type.c_str());
	}
}

void EditAsset(const Args& args)
{			
	std::unordered_map<std::string, std::function<void(const std::string&)>> assetToEditorMap = 
	{
		{
			Assets::GetAssetTypeExt<Graphics::Tileset>(),
			Sys::RunTilesetEditor
		},
		{
			Assets::GetAssetTypeExt<Graphics::Map>(),
			Sys::RunMapEditor
		},
		{
			Assets::GetAssetTypeExt<Script>(),
			Sys::RunScriptEditor
		}
		//TODO load font as tls
		//{
		//	Assets::GetAssetTypeExt<Graphics::Font>(),
		//	Sys::RunFontEditor
		//},
	};
	//edit <asset>   
	ARG_COUNT(args, 1);
	//clear previous asset paths
	// use current working directory
	Assets::ClearPaths();
	Assets::AddPath(FS::Cwd());
	//keep the dot
	const std::string& type = "." + FS::FileExt(args[0]);
	const std::string& name = FS::NoExt(args[0]);
	auto it = assetToEditorMap.find(type);
	if(it != assetToEditorMap.end())
	{
		it->second(name);
	}
	else
	{
		SHELL_LOG("Could load asset type (%s)", type.c_str() );
	}
}


