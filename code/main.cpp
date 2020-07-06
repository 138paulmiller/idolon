#include "core.h"
#include "assets.h"
#include "graphics.h"
#include "engine.h"
#include "shell.h"
#include "editsheet.h"
#include "fs.h"
#include "err.h"


enum ViewType
{
	VIEW_SHELL = 0,
	VIEW_EDIT_SHEET,
	VIEW_COUNT,
};

struct Context
{
	std::string sysPath;
	std::string sysAssetPath;
	//current working dir
	std::string workPath;
	ViewType prevView;
	ViewType view;
	UI * ui ;
};
//system context
Context g_context;
UI * g_ui[VIEW_COUNT];

template <typename Type>
Type * GetView(int index)
{
	return dynamic_cast<Type*>(g_ui[index]);
}

void SwitchView(ViewType view)
{
	if(g_context.ui)
		g_context.ui->onExit();
	
	g_context.prevView = g_context.view;
	g_context.ui = g_ui[view];
	g_context.view = view; 
	
	g_context.ui->onEnter();
}

void Startup(const Context & context)
{
	//default config
	g_context.sysPath = FS::ExePath() + "/system";
	g_context.sysAssetPath = g_context.sysPath + "/assets";
	g_context.workPath = FS::Root();
	g_context.ui = 0;


	Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
	//add system assets path
	Assets::Startup(context.sysAssetPath);
	
	g_ui[VIEW_SHELL]      = new Shell();
	g_ui[VIEW_EDIT_SHEET] = new EditSheet();

	SwitchView(VIEW_SHELL);
}

void Shutdown()
{
	if(	g_context.ui)
		g_context.ui->onExit();
	for(int i=0; i < VIEW_COUNT; i++)
		delete g_ui[i];

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
		Assets::Save(sheet);

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
	//clear previous asset paths
	// use current working directory
	Assets::ClearPaths();
	Assets::AddPath(g_context.workPath);
	
	//edit <asset>   
	ARG_COUNT(args, 1);
	const std::string& ext = FS::FileExt(args[0]);
	const std::string& name = FS::BaseName(args[0]);
	if(ext == "sheet")
	{
		EditSheet * editsheet = GetView<EditSheet>(VIEW_EDIT_SHEET); //references the
		editsheet->setSheet(name);
		SwitchView(VIEW_EDIT_SHEET);
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
			ARG_COUNT(args, 1) //

			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the
			shell->setFont(args[0]);
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
			std::string path = g_context.workPath;
			std::vector<std::string> files;
			path += (args.size() == 1) ? "/"+args[0] : "";
			FS::List(path, files);
			for (const std::string& file : files)
				shell->log(file);
		} 
	},
	{
		"cd", 
		[](Args args)
		{ 
			ARG_COUNT(args, 1) // 	
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the

			const std::string & root = FS::Root();
			const std::string & path = FS::Append(g_context.workPath, args[0]);
			//do not naviage beyond root. compare substring to see if new dir is subdir of root
			if ( path.compare(0, root.size(), root.c_str(), root.size()) != 0 )
				return;

			if (FS::IsDir(path))
				g_context.workPath = path;
			else
				shell->log("Directory does not exist");
		} 
	},
	{
		"cwd", 
		[](Args args)
		{ 
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the
			shell->log(g_context.workPath);
		} 
	},
	{
		"mkdir", 
		[](Args args)
		{ 
			Shell * shell = GetView<Shell>(VIEW_SHELL); //references the

			ARG_COUNT(args, 1) // 	
			const std::string & path = FS::Append(g_context.workPath, args[0]);
			if (!FS::MkDir(path))
				shell->log("Could not create directory");
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
				SwitchView(g_context.prevView);	
			else 
				g_context.ui->onKey(key, isDown);
		}
	);
	float timer = 0;
	while (Engine::Run())
	{
		g_context.ui->onTick();
	}
	Engine::SetKeyEcho(false);

	Shutdown();
	return 0;
}