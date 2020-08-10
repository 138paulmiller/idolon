

#include "sys.hpp"
#include "ui/api.hpp"
#include "scripting/eval.hpp"
#include "tileseteditor.hpp"
#include "mapeditor.hpp"

//TODO handle all exceptions here. Close down, or restart system if necessary. Worst case shutdown, then startup system.
namespace
{
	Shell  * s_shell;
	Context* s_context;
	
	//default config
	//system are for default system data
	std::string s_sysPath ;
	std::string s_sysAssetPath;
} // namespace

namespace Sys
{

	void Startup( const CommandTable &cmds )
	{
		s_context = new Context( APP_COUNT );

		//default config
		s_sysPath = FS::ExePath() + "/system/";
		s_sysAssetPath = s_sysPath + "/assets/";
		char title[24];
        snprintf( title,24, "%s v%d.%d", SYSTEM_NAME, VERSION_MAJOR, VERSION_MINOR );
		
		Engine::Startup( title, SCREEN_W, SCREEN_H, WINDOW_SCALE );
		//add system assets path
		Assets::Startup( Sys::AssetPath() );

		Eval::Startup();

		s_context->create( APP_SHELL, s_shell = new Shell() );
		s_context->create( APP_TILESET_EDITOR, new TilesetEditor() );
		s_context->create( APP_MAP_EDITOR, new MapEditor() );

		Engine::SetKeyEcho( true );
		Engine::PushKeyHandler(
			[&] ( Key key, bool isDown )
			{
				if ( isDown && key == KEY_ESCAPE )
					s_context->exit();
				else
					s_context->handleKey( key, isDown );
			}
		);
		//boot into shell
		Sys::GetContext()->enter( APP_SHELL );

		s_shell->addCommands( cmds );

		LOG( "System On!\n" );
	}

	void Shutdown()
	{
		s_context->clear();
		delete s_context;

		Eval::Shutdown();
		LOG( "Shutting down assets ...\n" );

		Assets::Shutdown();

		LOG( "Shutting down engine ...\n" );
		Engine::Shutdown();

		LOG( "Goodbye :)\n" );
		exit( 1 );
	}

	int Run()
	{
		Eval::Execute(
			R"(
from time import time,ctime

print('Today is', ctime(time()))

import idolon as I

mouse = I.mouse()
print("Mouse:%s" % (mouse))
	   	)" );
		//import test py functions
		Eval::Compile( "game" );

		TypedArg ret( ARG_STRING );
		Eval::Call( "multiply", { 3, 2 }, ret );
		LOG( "Call returned %s\n", ret.value.s );

		while ( Engine::Run() )
		{
			switch ( Sys::GetContext()->run() )
			{
			case APP_CODE_CONTINUE:
				break;
			case APP_CODE_SHUTDOWN:
				Sys::Shutdown();
				break;
			case APP_CODE_EXIT:
				//set current back to conitnue so can reenter
				s_context->app()->signal( APP_CODE_CONTINUE );
				s_context->exit();
				break;
			}
			//stat fps 
			static float timer = 0;
			timer += Engine::GetTimeDeltaMs()/1000.0f;
			//every 1 seconds
			if (timer > FPS_STAT_RATE) 
			{
				printf("FPS:%.2f\n", Engine::GetFPS());
				timer = 0.f;
			}

		}
		//
		Shutdown();
		return 0;
	}

	const std::string &Path()
	{
		return s_sysPath;
	}

	const std::string &AssetPath()
	{
		return s_sysAssetPath;
	}

	Shell *GetShell()
	{
		return s_shell;
	}

	Context *GetContext()
	{
		return s_context;
	}

	void RunTilesetEditor( const std::string &tilesetName )
	{
		s_context->app<TilesetEditor>(APP_TILESET_EDITOR)->setTileset(tilesetName);
		s_context->enter(APP_TILESET_EDITOR);
	}
	
	void RunMapEditor( const std::string &mapName )
	{
		s_context->app<MapEditor>(APP_MAP_EDITOR)->setMap(mapName);
		s_context->enter(APP_MAP_EDITOR);
	}

} // namespace Sys