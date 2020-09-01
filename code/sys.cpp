

#include "sys.hpp"
#include "game.hpp"
#include "ui/api.hpp"
#include "editors/api.hpp"

//TODO handle all exceptions here. Close down, or restart system if necessary. Worst case shutdown, then startup system.
namespace
{
	Shell  * s_shell;
	Context* s_context;
	
	std::list<Factory *>s_factories;

	//default config
	//system are for default system data
	std::string s_sysPath ;
	std::string s_sysAssetPath;

	GameState s_gamestate;

} // namespace

namespace Sys
{

	void Startup( const CommandTable &cmds )
	{
		char title[24];
        snprintf( title,24, "%s v%d.%d", SYSTEM_NAME, VERSION_MAJOR, VERSION_MINOR );

		s_gamestate = GAME_OFF;
		s_context = new Context( APP_COUNT );

		//default config
		s_sysPath = FS::ExePath() + "/system/";
		s_sysAssetPath = s_sysPath + "/assets/";


		Engine::Startup( title, SCREEN_W, SCREEN_H, WINDOW_SCALE );
		Assets::Startup( Sys::AssetPath() );
		Eval::Startup();

		//setup context apps
		s_context->create( APP_SHELL,          s_shell = new Shell() );
		s_context->create( APP_TILESET_EDITOR, new TilesetEditor()   );
		s_context->create( APP_MAP_EDITOR,     new MapEditor()       );
		s_context->create( APP_SCRIPT_EDITOR,  new ScriptEditor()    );
		s_shell->addCommands( cmds );

		//redirect key event to appropriate app. 
		//handle system level keys. i.e escape and quick shell toggle (tilda ? )
		Engine::SetKeyEcho( true );
		Engine::PushKeyHandler(
			[&] ( Key key, bool isDown )
			{
				if ( isDown && key == KEY_ESCAPE )
				{	
					s_context->app()->signal( APP_CODE_CONTINUE ); //allow reenter
					s_context->exit();
				}
				else
				{
					s_context->handleKey( key, isDown );
				}
			}
		);

		//boot into shell
		Sys::GetContext()->enter( APP_SHELL );


		LOG( "System On!\n" );
	}

	void Shutdown()
	{
		
		s_context->clear();
		delete s_context;

		Eval::Shutdown();
		Assets::Shutdown();
		Engine::Shutdown();

		LOG( "System Off!\n" );
		exit( 1 );
	}

	static void EditorStep()
	{
		//run editor context
		switch ( Sys::GetContext()->run() )
		{
		case APP_CODE_CONTINUE:
			break;
		case APP_CODE_SHUTDOWN:
			Sys::Shutdown();
			break;
		case APP_CODE_EXIT:
			//cannot exit shell.
			//set current back to continue so can reenter
			s_context->app()->signal( APP_CODE_CONTINUE );
			s_context->exit();
			break;
		}

	}


	static void GameStep()
	{
		TypedArg ret;
		s_gamestate = Game::Update(); 
		if(s_gamestate == GAME_OFF)
		{
			Game::Shutdown();
		}
	}

	int Run()
	{
		while ( Engine::Run() )
		{
			switch(s_gamestate)
			{
				case GAME_OFF: 
				case GAME_PAUSED: 
					EditorStep();
				break;
				case GAME_RUNNING:
					GameStep();
				break;
			}

		}
		//stat fps 
		static float timer = 0;
		timer += Engine::GetTimeDeltaMs()/1000.0f;
		//every 1 seconds
		if (timer > FPS_STAT_RATE) 
		{
			LOG("FPS:%.2f\n", Engine::GetFPS());
			timer = 0.f;
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
	
	void RunScriptEditor(const std::string & scriptName)
	{
		s_context->app<ScriptEditor>(APP_SCRIPT_EDITOR)->setScript(scriptName);
		s_context->enter(APP_SCRIPT_EDITOR);
	}

	void RunGame(const std::string & gameName)
	{
		TypedArg ret;
		Game::Startup(gameName);
	}
} // namespace Sys