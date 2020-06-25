#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "editor.h"
#include <functional>


#define GRID_W 200
#define GRID_H 150
#define WINDOW_SCALE 1.0/5
#define SIM_RATEMS 10


//action to load 


void renderCheck0(int w, int h, float scale)
{
	float timer = 10;
	int texture = 0;

	while (Engine::Run())
	{
		timer -= Engine::GetTimeDeltaMs()/1000.0;
		//every 1 seconds
		if (timer <= 0) 
		{
			printf("FPS:%.2f\n", Engine::GetFPS());
			timer = 10;
		}

		//perf test. create destory
		int texture = Engine::CreateTexture(w, h);
		const int len = w * h;
		Color* pixels = Engine::LockTexture(texture, { 0,0,w,h });

		for(int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			const int i = y * w + x;

			Color& pixel = pixels[i];
			pixel.r = ((float)x) / w * 255;
			pixel.g = ((float)y) / h*255;
			pixel.b = 0;
			pixel.a = 255;
		}
		Engine::UnlockTexture(texture);
		Engine::DrawTexture(texture, { 0,0,w,h}, { 0,0,w,h});
		Engine::DestroyTexture(texture);
	}
}

using Args = std::vector<std::string > ;
using Command = std::function<void(Args)>;
using CommandTable = std::map<std::string, Command>;  
void Execute(int argc, char** argv, const CommandTable & commands ) 
{
	Args args; 
	//ignore first arg
	int i = 1;
	while( i < argc)
	{
		CommandTable::const_iterator it = commands.find(argv[i]);
		if(it == commands.end())
		{
			printf("Execute: Invalid Command (%s)\n", argv[i]);
			return;
		}
		//parse until non cmd is found
		const Command & command  = it->second; 
		i++;
		while(i < argc)
		{
			const char *arg=  argv[i];
			it = commands.find(arg);
			if (it == commands.end()) //if does not exist
				args.push_back(arg);
			else
				break;
			i++;
		}
		command(args);
		args.clear();
	}
}


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

Graphics::Sheet * ConvertImageToAsset(const std::string & filepath)
{
	int w, h;
	Color * pixels = Engine::LoadTexture(filepath, w, h);
	std::string name = GetFilename(filepath);
	Graphics::Sheet * sheet = new Graphics::Sheet(name, w, h);
	memcpy(sheet->pixels, pixels, w * h * sizeof(Color));
	sheet->update();
	return sheet;
}

////create usage messages
//[min,max] 
#define ARG_RANGE(args, min, max) assert(args.size() >= min && args.size() <= max );
int main(int argc, char** argv)
{ 
	std::string root = "./";
	std::vector<const Graphics::Sheet * > convertedSheets;
	CommandTable cli = {
		{ 	"-game", 
			[&](Args args){ ARG_RANGE(args, 0, 1) root = args[0]; } 
		},
		{ 	"-convert", 
			[&](Args args)
			{ 
				ARG_RANGE(args, 0, 1) 
				convertedSheets.push_back(ConvertImageToAsset(args[0])); 
			} 
		}  
	};
	
	Engine::Startup(GRID_W, GRID_H, WINDOW_SCALE);

	Execute(argc, argv, cli);
	Assets::Startup(root);
	//Save converted assets
	for ( int i =0; i < convertedSheets.size(); i++)
	{
		const Graphics::Sheet* sheet = convertedSheets[i];
		Assets::Save(sheet, sheet->name);
	}
	
	printf("Config:\n\tAsset Dir : %s\n", root.c_str());
	
	Editor::RunSheetView("example");
	Assets::Shutdown();

	Engine::Shutdown();

	return 0;
}