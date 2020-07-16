#include "sys.h"
//
#include "sheeteditor.h"

namespace
{
	Shell  * s_shell;
	Editor * s_editor;
	UI::Toolbar * s_menu;
	Context* s_context;
	
	//default config
	//system are for default system data
	std::string s_sysPath ;
	std::string s_sysAssetPath;
}

namespace Sys
{
	
	void Startup(const CommandTable & cmds)
	{
		s_context = new Context(APP_COUNT);

		//default config
		s_sysPath = FS::ExePath() + "/system";
		s_sysAssetPath = s_sysPath + "/assets";

		Engine::Startup(SCREEN_W, SCREEN_H, WINDOW_SCALE);
		//add system assets path
		Assets::Startup(Sys::AssetPath());

		printf("System On!\n");

		s_context->create(APP_SHELL, s_shell = new Shell());
		s_context->create(APP_SHEET_EDITOR, s_editor = new SheetEditor());
	
		Engine::SetKeyEcho(true);
		Engine::SetKeyHandler(
			[&](Key key, bool isDown) 
			{
				if(isDown && key == KEY_ESCAPE)
					s_context->exit();	
				else 
					s_context->handleKey(key, isDown);
			}
		);
		//boot into shell
		Sys::GetContext()->enter(APP_SHELL);	

		s_shell->addCommands(cmds);

	}
	
	void Shutdown()
	{
		s_context->clear();
		printf("Shutting down assets ...\n");
		Assets::Shutdown();
		printf("Shutting down engine ...\n");
		Engine::Shutdown();
		delete s_context;
		printf("Goodbye :)\n");
		exit(1);
	}

	int Run()
	{
		while (Engine::Run())
		{
			switch(Sys::GetContext()->run())
			{
				case APP_CODE_CONTINUE:
					break;
				case APP_CODE_SHUTDOWN:
					Sys::Shutdown();
					break;
				case APP_CODE_EXIT:
					s_context->app()->signal( APP_CODE_CONTINUE );
					s_context->exit();
					break;
			}
		}
		//
		Shutdown();
		return 0;
	}

	const std::string & AssetPath() 
	{
		return s_sysAssetPath;
	}
		
	Shell* GetShell()
	{
		return s_shell;
	}

	Editor* GetEditor()
	{
		return s_editor;
	}

	UI::Toolbar* GetMenu() 
	{
		return s_menu;
	}
	
	Context* GetContext()
	{
		return s_context;
	}

}