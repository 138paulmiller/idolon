#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "shell.h"
#include "fs.h"
#include "err.h"
//editor
#include "sheeteditor.h"


enum ViewId
{
	VIEW_SHELL = 0,
	VIEW_SHEET_EDITOR,
	VIEW_COUNT,
};

struct Context
{
	std::string sysPath;
	std::string sysAssetPath;
	//current working dir
	ViewId prevViewId;
	ViewId viewId;
	UI::Widget * view ;
};
//system context
Context g_context;
UI::Widget * g_views[VIEW_COUNT];

template <typename Type>
Type * GetView(int index)
{
	return dynamic_cast<Type*>(g_views[index]);
}

void SwitchView(ViewId view)
{
	if(g_context.view)
		g_context.view->onExit();
	
	g_context.prevViewId = g_context.viewId;
	g_context.view = g_views[view];
	g_context.viewId = view; 
	
	g_context.view->onEnter();
}

void Startup(const Context & context)
{
	//default config
	//system are for default system data
	g_context.sysPath = FS::ExePath() + "/system";
	g_context.sysAssetPath = g_context.sysPath + "/assets";
	//root is in user space 
	g_context.view = 0;


	Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
	//add system assets path
	Assets::Startup(context.sysAssetPath);
	
	Shell *shell = new Shell();

	g_views[VIEW_SHELL]      = shell;
	g_views[VIEW_SHEET_EDITOR] = new SheetEditor();

	SwitchView(VIEW_SHELL);

}

void Shutdown()
{
	if(	g_context.view)
		g_context.view->onExit();
	for(int i=0; i < VIEW_COUNT; i++)
		delete g_views[i];

	Assets::Shutdown();
	Engine::Shutdown();
	printf("Goodbye :)");
	exit(1);
}

void PrintHelp()
{

	Shell * shell = GetView<Shell>(VIEW_SHELL); //references the
	shell->log("Commands");
	shell->log("help - print this message");
	shell->log("exit - power down system");
	shell->log("font <name> - set shell font");
}

void LogArgError()
{
	char msg[24];
	snprintf(msg, 24, "Incorrect number of arguments");
	GetView<Shell>(VIEW_SHELL)->log(msg);
}


//TODO - handle error more elegantly. Throw exception with msg ? Log mesage to shell ? with usage. 
#define ARG_NONEMPTY(args) if(args.size() <= 0  ) { LogArgError(); return; }
#define ARG_RANGE(args, min, max) if(args.size() < min || args.size() > max ) { LogArgError(); return; }
#define ARG_COUNT(args, i) if(args.size() != i ){ LogArgError(); return; }



template <typename Type>
std::string SystemAssetPath(const std::string & name )
{
	return g_context.sysAssetPath + ("/" + name + Assets::GetAssetTypeExt<Type>());
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
		SheetEditor * editsheet = GetView<SheetEditor>(VIEW_SHEET_EDITOR); //references the
		editsheet->setSheet(name);
		SwitchView(VIEW_SHEET_EDITOR);
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
			GetView<Shell>(VIEW_SHELL)->setFont(args[0]);
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
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the
			std::vector<std::string> files;
			FS::Ls(files);
			for (const std::string& file : files)
				shell->log(file);
		} 
	},
	{
		"cd", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1); // 	
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the
			if (!FS::Cd(args[0]))
				shell->log("Directory does not exist");
		} 
	},
	{
		"cwd", 
		[](Args args)
		{ 
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the
			shell->log(FS::Cwd());
		} 
	},
	{
		"mkdir", 
		[](Args args)
		{ 
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the

			ARG_COUNT(args, 1); // 	
			if (!FS::MkDir(args[0]))
				shell->log("Could not create directory");
		} 
	},
	{
		"rm", 
		[](Args args)
		{ 
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the

			ARG_COUNT(args, 1); // 	
			if (!FS::Remove(args[0]))
				shell->log("Failed to remove path");
		} 
	},
	{
		"mv", 
		[](Args args)
		{ 
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the

			ARG_COUNT(args, 2); // 
			
			if (!FS::Move(args[0], args[1]))
				shell->log("Failed to move");
		} 
	},
	//if running game, add all sub-directories as search dir for assets
	//on exit, clear the asset paths (will remove all but system)
};
	
//----------------------------


int main(int argc, char** argv)
{ 

	stacktrace();

	Startup(g_context);
	Shell * shell = GetView<Shell>(VIEW_SHELL); //references the

	shell->addCommands(shellcommands);

	//TODO - capture all input here, forward it to the shell 
	// "trampoline" contexts. 
	//Create base "class View  for shell and editors. the view will return contexts. 
	//also, each context will register its commands 
	//Editor views, Shell will return a code that indicates the action to take. 
	//ie switch to another context, exit, or run game.  
	
	Engine::SetKeyEcho(true);
	Engine::SetKeyHandler(
		[&](Key key, bool isDown) 
		{
			if(isDown && key == KEY_ESCAPE)
				SwitchView(g_context.prevViewId);	
			else 
				g_context.view->onKey(key, isDown);
		}
	);
	float timer = 0;
	while (Engine::Run())
	{
		g_context.view->update();
		g_context.view->onTick();
		//draw ui layer on top
		g_context.view->draw();
	}
	Engine::SetKeyEcho(false);

	Shutdown();
	return 0;
}