#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "shell.h"
#include "editor.h"

//Default settings
#define FONT_W 7
#define FONT_H 9
#define SCREEN_W 320
#define SCREEN_H 270
#define WINDOW_SCALE 1.0/2
#define SYS_ASSET_PATH "data/system/assets"

void Startup()
{
	Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
	Assets::Startup(SYS_ASSET_PATH);
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
	Shell::Log("quit - power down system");
	Shell::Log("font <name> - set shell font");
}

//TODO - handle error more elegantly. Throw exception with msg ? 
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
	return SYS_ASSET_PATH + ("/" + name + Assets::GetAssetTypeExt<Type>());
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

// Commands --------------------------------- 

// todo remove args commands
CommandTable argcommands = 
{
	{
		"-exit", 
		[](Args args)
		{ 
			Shutdown();
		} 
	},
	{ 	
		"-create", 
		[](Args args)
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
				int start = std::stoi(args[4]);
				Graphics::Font * font = CreateFontAsset(img, cw, ch, start);
				Assets::Save(font, font->name);
			}
		} 
	}  
};



CommandTable shellcommands = 
{
	{ 	
		"help", 
		[](Args args)
		{ 
			PrintHelp();
		},
	},
	{
		"quit", 
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
	}
};
	
//----------------------------


int main(int argc, char** argv)
{ 
	Startup();
	Shell::AddCommands(shellcommands);

	//ignore first arg (program path)
	argv++; argc--;
	Execute(argc, argv, argcommands);
	Shell::Run();
	Shutdown();
	return 0;
}