#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "shell.h"
#include "editor.h"
#include "fs.h"

//Default settings
#define FONT_W 7
#define FONT_H 9
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
Context context;

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

std::string GetFilename(std::string path)
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

std::string GetDir(std::string path)
{
    size_t sep = path.find_last_of("\\/");
    if (sep != std::string::npos)
        return path.substr(0, sep);
    return "";
}

template <typename Type>
std::string SystemAssetPath(const std::string & name )
{
	return context.sysAssetPath + ("/" + name + Assets::GetAssetTypeExt<Type>());
}

Graphics::Font * CreateFontAsset(const std::string & filepath, int cw, int ch, char start)
{
	int w, h;
	Color * pixels = Engine::LoadTexture(filepath, w, h);
	std::string name = GetFilename(filepath);
	Graphics::Font * font = new Graphics::Font(name, w, h, cw, ch, start);
	memcpy(font->pixels, pixels, w * h * sizeof(Color));
	font->update();
	font->filepath = SystemAssetPath<Graphics::Font>(name);
	return font;
}

Graphics::Sheet * CreateSheetAsset(const std::string & filepath)
{
	int w, h;
	Color * pixels = Engine::LoadTexture(filepath, w, h);
	std::string name = GetFilename(filepath);
	Graphics::Sheet * sheet = new Graphics::Sheet(name, w, h);
	memcpy(sheet->pixels, pixels, w * h * sizeof(Color));
	sheet->update();
	sheet->filepath = SystemAssetPath<Graphics::Sheet>(name);
	return sheet;
}

void CreateAsset(const Args& args)
{			
	ARG_NONEMPTY(args)

	if(args[0].compare("sheet") == 0)
	{
		//must be two args
		ARG_COUNT(args, 2) 
		Graphics::Sheet * sheet = CreateSheetAsset(args[1]);
		Assets::Save(sheet, sheet->name);

	}
	else if(args[0].compare("font") == 0)
	{
		ARG_COUNT(args, 5) // 
		const std::string &  img = args[1];
		int cw = std::stoi(args[2]);
		int ch = std::stoi(args[3]);
		//charcter offset (ascii value)
		int start = std::stoi(args[4]);
		Graphics::Font * font = CreateFontAsset(img, cw, ch, start);
		Assets::Save(font, font->name);
	}
}

// Commands --------------------------------- 


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
		"create", 
		CreateAsset
	},
	{
		"ls", 
		[](Args args)
		{ 
			std::string path = context.workPath;
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
			const std::string & path = FS::FullPath(context.workPath + "/" + args[0]);
			//do not naviage beyond root. compare substring to see if new dir is subdir of root
			if ( path.compare(0, root.size(), root.c_str(), root.size()) != 0 )
				return;

			if (FS::IsDir(path))
				context.workPath = path;
			else
				Shell::Log("Directory does not exist");
		} 
	},
	{
		"cwd", 
		[](Args args)
		{ 
			Shell::Log(context.workPath);
		} 
	},
	{
		"mkdir", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1) // 	
			std::string path = FS::FullPath(context.workPath + "/" + args[0]);
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
	context.sysPath = FS::ExePath() + "/system";
	context.sysAssetPath = context.sysPath + "/assets";
	//enter 
	context.workPath = FS::Root();

	//prefix all cmdline args with -
	//do this to build the config file. 
	//Execute(argc, argv, shellcommands);

	Startup(context);
	Shell::AddCommands(shellcommands);
	Shell::Run();
	Shutdown();
	return 0;
}