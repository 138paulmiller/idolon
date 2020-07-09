#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "shell.h"
#include "fs.h"
#include "err.h"
//editor
#include "shell.h"
#include "editor.h"
#include "context.h"

//default config
//system are for default system data
std::string g_sysPath ;
std::string g_sysAssetPath;

enum : uint8_t
{
	APP_SHELL = 0,
	APP_EDITOR,
	APP_COUNT
};


Shell  * g_shell;
Editor * g_editor;
Context g_context(APP_COUNT);


void Startup()
{
	//default config
	g_sysPath = FS::ExePath() + "/system";
	g_sysAssetPath = g_sysPath + "/assets";



	Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
	//add system assets path
	Assets::Startup(g_sysAssetPath);

	g_context.create(APP_SHELL, g_shell = new Shell());
	g_context.create(APP_EDITOR, g_editor = new Editor());
}


void Shutdown()
{
	Assets::Shutdown();
	Engine::Shutdown();
	printf("Goodbye :)\n");
	exit(1);
}

void PrintHelp()
{

	g_shell->log("Commands");
	g_shell->log("help - print this message");
	g_shell->log("exit - power down system");
	g_shell->log("font <name> - set shell font");
}

void LogArgError()
{
	char msg[24];
	snprintf(msg, 24, "Incorrect number of arguments");
	g_shell->log(msg);
}


//TODO - handle error more elegantly. Throw exception with msg ? Log mesage to shell ? with usage. 
#define ARG_NONEMPTY(args) if(args.size() <= 0  ) { LogArgError(); return; }
#define ARG_RANGE(args, min, max) if(args.size() < min || args.size() > max ) { LogArgError(); return; }
#define ARG_COUNT(args, i) if(args.size() != i ){ LogArgError(); return; }



template <typename Type>
std::string SystemAssetPath(const std::string & name )
{
	return g_sysAssetPath + ("/" + name + Assets::GetAssetTypeExt<Type>());
}
// -------------- Convert raw to asset ------------------------


void ConvertAsset(const Args& args)
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
		g_context.enter(APP_EDITOR);
		g_editor->editSheet(name);
	}
	else if(ext == "font")
	{
	//	SwitchView(g_context, VIEW_FONT_EDITOR);
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
			ARG_COUNT(args, 1);
			g_shell->setFont(args[0]);
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
			std::vector<std::string> files;
			FS::Ls(files);
			for (const std::string& file : files)
				g_shell->log(file);
		} 
	},
	{
		"cd", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::Cd(args[0]))
				g_shell->log("Directory does not exist");
		} 
	},
	{
		"cwd", 
		[](Args args)
		{ 
			g_shell->log(FS::Cwd());
		} 
	},
	{
		"mkdir", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::MkDir(args[0]))
				g_shell->log("Could not create directory");
		} 
	},
	{
		"rm", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			if (!FS::Remove(args[0]))
				g_shell->log("Failed to remove path");
		} 
	},
	{
		"mv", 
		[](Args args)
		{ 
			ARG_COUNT(args, 2); // 
			
			if (!FS::Move(args[0], args[1]))
				g_shell->log("Failed to move");
		} 
	},
	//if running game, add all sub-directories as search dir for assets
	//on exit, clear the asset paths (will remove all but system)
};
	
//----------------------------


int main(int argc, char** argv)
{ 

	stacktrace();
	Startup();
	
	g_shell->addCommands(shellcommands);

	g_context.enter(APP_SHELL);	

	//Editor views, Shell will return a code that indicates the action to take. 
	//ie switch to another context, exit, or run game.  
	
	Engine::SetKeyEcho(true);
	Engine::SetKeyHandler(
		[&](Key key, bool isDown) 
		{
			if(isDown && key == KEY_ESCAPE)
				g_context.exit();	
			else 
				g_context.handleKey(key, isDown);
		}
	);
	float timer = 0;
	while (Engine::Run())
	{
		g_context.run();
	}
	Engine::SetKeyEcho(false);

	Shutdown();
	return 0;
}