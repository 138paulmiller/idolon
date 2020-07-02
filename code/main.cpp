#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "shell.h"
#include "editor.h"
#include "fs.h"

//Default settings
//TODO make font 8x8
#define FONT_W 7
#define FONT_H 9
//Default Sheet size
#define SHEET_W 128
#define SHEET_H 128
//Default Sheet size
#define TILE_W 16
#define TILE_H 16

#define SCREEN_W 320
#define SCREEN_H 270
#define WINDOW_SCALE 1.0/2


struct Context
{
	std::string sysPath;
	std::string sysAssetPath;
	//current working dir
	std::string workPath;
};
//system context
Context g_context;

void Startup(const Context & context)
{
	Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
	//add system assets path
	Assets::Startup(context.sysAssetPath);
	Shell::Startup();
}

void Shutdown()
{
	Shell::Shutdown();
	Assets::Shutdown();
	Engine::Shutdown();
	printf("Goodbye :)");
	exit(1);
}

void PrintHelp()
{
	Shell::Log("Commands");
	Shell::Log("help - print this message");
	Shell::Log("exit - power down system");
	Shell::Log("font <name> - set shell font");
}

//TODO - handle error more elegantly. Throw exception with msg ? Log mesage to shell ? with usage. 
#define ARG_NONEMPTY(args) assert(args.size() > 0  );
#define ARG_RANGE(args, min, max) assert(args.size() >= min && args.size() <= max );
#define ARG_COUNT(args, i) assert(args.size() == i );



template <typename Type>
std::string SystemAssetPath(const std::string & name )
{
	return g_context.sysAssetPath + ("/" + name + Assets::GetAssetTypeExt<Type>());
}
// -------------- Convert raw to asset ------------------------


void ConvertAsset(const Args& args)
{			
	ARG_NONEMPTY(args)

	if(args[0] == "sheet" )
	{
		//must be two args
		ARG_COUNT(args, 2) 
		int w, h;
		const std::string &  imgpath = args[1];
		Color * pixels = Engine::LoadTexture(imgpath, w, h);
		std::string name = FS::BaseName(imgpath);
		Graphics::Sheet * sheet = new Graphics::Sheet(name, w, h);
		memcpy(sheet->pixels, pixels, w * h * sizeof(Color));
		sheet->update();
		sheet->filepath = SystemAssetPath<Graphics::Sheet>(name);
		Assets::Save(sheet, sheet->name);

	}
	else if(args[0] == "font" )
	{
		ARG_COUNT(args, 5) // 
		const std::string &  imgpath = args[1];
		int cw = std::stoi(args[2]);
		int ch = std::stoi(args[3]);
		//charcter offset (ascii value)
		int start = std::stoi(args[4]);
		int w, h;
		Color * pixels = Engine::LoadTexture(imgpath, w, h);
		std::string name = FS::BaseName(imgpath);
		Graphics::Font * font = new Graphics::Font(name, w, h, cw, ch, start);
		memcpy(font->pixels, pixels, w * h * sizeof(Color));
		font->update();
		font->filepath = SystemAssetPath<Graphics::Font>(name);
		Assets::Save(font, font->name);
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
		const std::string & path = FS::Append(g_context.workPath, sheet->name) + Assets::GetAssetTypeExt<Graphics::Sheet>();
		Assets::SaveAs(sheet, path);
	}
	else if(asset == "font")
	{
		//8x8 font 
		int w = 6 * 8;
		int h = 18 * 8;
		char start = ' ';
		Graphics::Font* font= new Graphics::Font(name, w, h, 8, 8, start);
		const std::string & path = FS::Append(g_context.workPath, font->name) + Assets::GetAssetTypeExt<Graphics::Font>();
		Assets::SaveAs(font, path);
	}
}

void EditAsset(const Args& args)
{			
	//edit <asset>   
	ARG_COUNT(args, 1);
	const std::string& ext = FS::FileExt(args[0]);
	const std::string& name = FS::BaseName(args[0]);
	if(ext == "sheet")
	{
		Editor::EditSheet(name);
	}
	else if(ext == "font")
	{
		Editor::EditFont(name);
	}
}


// Commands --------------------------------- 

/*
	load <game>.ult
	- loads game code as well as map/sheet/sprite editor. 
	- you can have up to 4 sheets. And A few maps. Can select assets to be used.  

*/

const CommandTable & shellcommands = 
{
	{ 	
		"help", 
		[](Args args)
		{ 
			PrintHelp();
		},
	},
	{
		"exit", 
		[](Args args)
		{ 
			Shutdown();
		} 
	},
	{
		"font", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1) // 	
			Shell::SetFont(args[0]);
		} 
	},
	{
		"convert", 
		ConvertAsset
	},
	{
		"new", 
		NewAsset
	},
	{
		"edit", 
		EditAsset
	},
	{
		"ls", 
		[](Args args)
		{ 
			std::string path = g_context.workPath;
			std::vector<std::string> files;
			path += (args.size() == 1) ? "/"+args[0] : "";
			FS::List(path, files);
			for (const std::string& file : files)
				Shell::Log(file);
		} 
	},
	{
		"cd", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1) // 	
			const std::string & root = FS::Root();
			const std::string & path = FS::Append(g_context.workPath, args[0]);
			//do not naviage beyond root. compare substring to see if new dir is subdir of root
			if ( path.compare(0, root.size(), root.c_str(), root.size()) != 0 )
				return;

			if (FS::IsDir(path))
				g_context.workPath = path;
			else
				Shell::Log("Directory does not exist");
		} 
	},
	{
		"cwd", 
		[](Args args)
		{ 
			Shell::Log(g_context.workPath);
		} 
	},
	{
		"mkdir", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1) // 	
			const std::string & path = FS::Append(g_context.workPath, args[0]);
			if (!FS::MkDir(path))
				Shell::Log("Could not create directory");
		} 
	},
	//if running game, add all sub-directories as search dir for assets
	//on exit, clear the asset paths (will remove all but system)
};
	
//----------------------------


int main(int argc, char** argv)
{ 
	//default config
	g_context.sysPath = FS::ExePath() + "/system";
	g_context.sysAssetPath = g_context.sysPath + "/assets";
	g_context.workPath = FS::Root();

	//prefix all cmdline args with -
	//do this to build the config file. 
	//Execute(argc, argv, shellcommands);

	Startup(g_context);
	Shell::AddCommands(shellcommands);
	Shell::Run();
	Shutdown();
	return 0;
}