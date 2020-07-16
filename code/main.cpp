#include "sys.h"
#include "sheeteditor.h"

void PrintHelp(const Args& args);
void ImportAsset(const Args& args);
void NewAsset(const Args& args);
void EditAsset(const Args& args);

void LogArgError()
{
	char msg[24];
	snprintf(msg, 24, "Incorrect number of arguments");
	Sys::GetShell()->log(msg);
}
//TODO - handle error more elegantly. Throw exception with msg ? Log mesage to shell ? with usage. 
#define ARG_NONEMPTY(args) if(args.size() <= 0  ) { LogArgError(); return; }
#define ARG_RANGE(args, min, max) if(args.size() < min || args.size() > max ) { LogArgError(); return; }
#define ARG_COUNT(args, i) if(args.size() != i ){ LogArgError(); return; }

/*
	load <game>.ult
	- loads game code as well as map/sheet/sprite editor. 
	- you can have up to 4 sheets. And A few maps. Can select assets to be used.  

*/
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
		{ "sys", "id [args] system settings" }, 
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
		{ "import", "asset name.png - create asset from file"}, 
		ImportAsset
	},
	{
		{ "new", "asset : create asset"} ,
		NewAsset
	},
	{
		{ "edit", "name.asset"}, 
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
				Sys::GetShell()->log(file);
		} 
	},
	{
		{ "cd", "dir : change dir"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::Cd(args[0]))
				Sys::GetShell()->log("Directory does not exist");
		} 
	},
	{
		{ "cwd", "print current path"}, 
		[](Args args)
		{ 
			Sys::GetShell()->log(FS::Cwd());
		} 
	},
	{
		{ "mkdir", "dir : create directory"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::MkDir(args[0]))
				Sys::GetShell()->log("Could not create directory");
		} 
	},
	{
		{ "rm", "dir : remove directory"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::Remove(args[0]))
				Sys::GetShell()->log("Failed to remove path");
		} 
	},
	{
		{ "mv", "path newpath : move to new path"}, 
		[](Args args)
		{ 
			ARG_COUNT(args, 2); // 
			
			if (!FS::Move(args[0], args[1]))
				Sys::GetShell()->log("Failed to move");
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


int main(int argc, char** argv)
{ 
	argc--; argv++;	//skip exe path arg

	Err::Stacktrace();

	Sys::Startup(g_cmds);

	Execute( argc, argv, g_cli );

	Sys::GetShell()->log("idolon v0.0");
	Sys::GetShell()->log("type help to see commands");

	return Sys::Run();
}

////////////////// Commands  ///////////////////////////////////////


template <typename Type>
std::string SystemAssetPath(const std::string & name )
{
	return Sys::AssetPath() + ("/" + name + Assets::GetAssetTypeExt<Type>());
}

void PrintHelp(const Args& args)
{
	std::string out;

	if(args.size()==1)
		Help(g_cmds, args[0], out);
	else 
		HelpAll(g_cmds, out);
	Sys::GetShell()->clear();
	Sys::GetShell()->log(out);
}

// -------------- Convert raw to asset ------------------------


void ImportAsset(const Args& args)
{			
	ARG_NONEMPTY(args);

	if(args[0] == "sheet" )
	{
		//must be two args
		ARG_COUNT(args, 2);
		int w, h;
		const std::string &  imgpath = args[1];
		Color * pixels = Engine::LoadTexture(imgpath, w, h);
		std::string name = FS::BaseName(imgpath);
		Graphics::Sheet * sheet = new Graphics::Sheet(name, w, h);
		memcpy(sheet->pixels, pixels, w * h * sizeof(Color));
		sheet->update();
		sheet->filepath = SystemAssetPath<Graphics::Sheet>(name);
		Assets::Save(sheet);

	}
	else if(args[0] == "font" )
	{
		ARG_COUNT(args, 5); // 
		const std::string &  imgpath = FS::Append(FS::Root(), args[1]);
		int cw = std::stoi(args[2]);
		int ch = std::stoi(args[3]);
		//charcter offset (ascii value)
		int start = std::stoi(args[4]);
		int w, h;
		Color * pixels = Engine::LoadTexture(imgpath, w, h);
		if ( !pixels ) return; //log err
		std::string name = FS::BaseName(imgpath);
		Graphics::Font * font = new Graphics::Font(name, w, h, cw, ch, start);
		memcpy(font->pixels, pixels, w * h * sizeof(Color));
		font->update();
		font->filepath = SystemAssetPath<Graphics::Font>(name);
		Assets::Save(font);
	}
}

void NewAsset(const Args& args)
{			 
	//new <asset>  <name>  
	ARG_COUNT(args, 2);
	const std::string& asset = args[0];
	const std::string& name = args[1];
	if(asset == "sheet")
	{
		Graphics::Sheet * sheet = new Graphics::Sheet(name, SHEET_W, SHEET_H);
		const std::string & path = FS::Append(FS::Cwd(), sheet->name) + Assets::GetAssetTypeExt<Graphics::Sheet>();
		memset(sheet->pixels, 255, sheet->w * sheet->h * sizeof(Color));
		sheet->update();
		Assets::SaveAs(sheet, path);
	}
	else if(asset == "font")
	{
		//8x8 font 
		int w = 6 * 8;
		int h = 18 * 8;
		char start = ' ';
		Graphics::Font* font= new Graphics::Font(name, w, h, 8, 8, start);
		const std::string & path = FS::Append(FS::Cwd(), font->name) + Assets::GetAssetTypeExt<Graphics::Font>();
		memset(font->pixels, 255, w * h * sizeof(Color));
		font->update();
		Assets::SaveAs(font, path);
	}
}

void EditAsset(const Args& args)
{			
	//clear previous asset paths
	// use current working directory
	Assets::ClearPaths();
	Assets::AddPath(FS::Cwd());
	
	//edit <asset>   
	ARG_COUNT(args, 1);
	const std::string& ext = FS::FileExt(args[0]);
	const std::string& name = FS::BaseName(args[0]);
	if(ext == "sheet")
	{
		Sys::GetContext()->app<SheetEditor>(APP_SHEET_EDITOR)->setSheet(name);
		Sys::GetContext()->enter(APP_SHEET_EDITOR);
	}
	else if(ext == "font")
	{
	}
}


