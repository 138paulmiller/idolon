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

Graphics::Font * CreateFontAsset(const std::string & filepath, int cw, int ch, char start)
{
	int w, h;
	Color * pixels = Engine::LoadTexture(filepath, w, h);
	std::string name = GetFilename(filepath);
	Graphics::Font * font = new Graphics::Font(name, w, h, cw, ch, start);
	memcpy(font->pixels, pixels, w * h * sizeof(Color));
	font->update();
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
	return sheet;
}

struct CreationRequests
{
	std::vector<const Graphics::Sheet* > sheets;
	std::vector<const Graphics::Font* > fonts;
};

void CreateAssets(const CreationRequests & requests)
{
	//Save converted assets to disk
	for ( int i =0; i < requests.sheets.size(); i++)
	{
		const Graphics::Sheet* sheet = requests.sheets[i];
		Assets::Save(sheet, sheet->name);
	}
	for ( int i =0; i < requests.fonts.size(); i++)
	{
		const Graphics::Font* font = requests.fonts[i];
		Assets::Save(font, font->name);
	}
}



#define ARG_NONEMPTY(args) assert(args.size() > 0  );
#define ARG_RANGE(args, min, max) assert(args.size() >= min && args.size() <= max );
#define ARG_COUNT(args, i) assert(args.size() == i );

void Startup()
{
}

void Shutdown()
{
	Shell::Shutdown();
	Assets::Shutdown();
	Engine::Shutdown();
	exit(1);
}

int main(int argc, char** argv)
{ 
	//ignore first arg (program path)
	argv++;
	argc--;
	std::string root = "./";
	CreationRequests requests;
	// todo remove args commands

	CommandTable argcommands = 
	{
		{ 	"-assets", 
			[&](Args args)
			{ 
				ARG_COUNT(args, 1) 
				root = args[0]; 
			} 
		},
		{ 	"-create", 
			[&](Args args)
			{ 
				ARG_NONEMPTY(args)
				if(args[0].compare("sheet") == 0)
				{
					//must be two args
					ARG_COUNT(args, 2) 
					requests.sheets.push_back(CreateSheetAsset(args[1])); 
				}
				else if(args[0].compare("font") == 0)
				{
					ARG_COUNT(args, 5) // 
					const std::string &  img = args[1];
					int cw = std::stoi(args[2]);
					int ch = std::stoi(args[3]);
					int start = std::stoi(args[4]);
					requests.fonts.push_back(CreateFontAsset(img, cw, ch, start)); 
				}
			} 
		}  
	};
	CommandTable shellcommands = 
	{
		{ 	"quit", 
			[&](Args args)
			{ 
				Shutdown();
			} 
		},
	};
	Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
	
	Execute(argc, argv, argcommands);
	
	Assets::Startup(root);
	CreateAssets(requests);
	Shell::Startup(shellcommands);
	
	Shell::Run();

	Shutdown();
	return 0;
}