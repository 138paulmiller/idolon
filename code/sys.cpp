
#include "sys.hpp"
#include "editors/api.hpp"

#include "idolon.hpp"

//TODO handle all exceptions here. Close down, or restart system if necessary. Worst case shutdown, then startup system.
namespace
{
	Game::Header  * s_header;
	Shell  * s_shell;
	Context* s_context;
	
	//default config
	//system are for default system data
	std::string s_sysPath ;
	std::string s_sysAssetPath;

	GameState s_gamestate;
	Factory * s_factories[FACTORY_COUNT];

	//editor
	MainMenu *s_menu;
} // namespace

void MainMenu::onEnter() 
{	
	m_appTab  = new UI::Toolbar( this,0, 0 );
	App::addWidget(m_appTab);
	m_appTab->font = DEFAULT_FONT_ICONS;

	m_appTab->add( TranslateIcon( "CODE" ), [this] () { 
		const std::string &name = (!s_header || s_header->scripts.empty()) ? "" : s_header->scripts[0];
		Sys::RunScriptEditor( name);	
	}, true, false );

	m_appTab->add( TranslateIcon( "TILESET" ), [this] () { 
		const std::string &name = (!s_header || s_header->tilesets.empty()) ? "" : s_header->tilesets[0];
		Sys::RunTilesetEditor( name);	
	}, true, false );

	m_appTab->add( TranslateIcon( "MAP" ), [this] () { 
		const std::string &name = (!s_header || s_header->maps.empty()) ? "" : s_header->maps[0];
		Sys::RunMapEditor( name);	
	}, true, false );

	int screenW, screenH;
	Engine::GetSize( screenW, screenH );
	m_controls = new UI::Toolbar( this,screenW, 0 );
	App::addWidget(m_controls);

	m_controls->font = DEFAULT_FONT_ICONS; //has glyphs
	m_controls->leftAlign = false;

	//BACK
	m_controls->add( TranslateIcon("EXIT"), [&](){
		Sys::RunShell( "" );
	}, false, false );

	m_controls->add( TranslateIcon("SAVE"), [&](){
		s_context->app()->onSave();
	}, false, false);
			
	m_controls->add( TranslateIcon("REDO"), [&](){
		s_context->app()->onRedo();
	}, false, false);
			
	m_controls->add( TranslateIcon("UNDO"), [&](){
		s_context->app()->onUndo();
	}, false, false);
			
}

void MainMenu::onExit() 
{
	App::clear();
}

void MainMenu::onKey( Key key, ButtonState state ) 
{}


void MainMenu::onTick() 
{}

void MainMenu::hide( bool isHidden )
{
	m_controls->hidden = isHidden;
	m_appTab->hidden = isHidden;
}

bool MainMenu::isHidden()
{
	return m_controls->hidden && m_appTab->hidden ;
}
UI::Toolbar *MainMenu::appTab()
{
	return m_appTab;
}

UI::Toolbar *MainMenu::controls()
{
	return m_controls;
}
namespace Sys
{

	void Startup( const CommandTable &cmds )
	{
		char title[24];
        snprintf( title,24, "%s v%d.%d", SYSTEM_NAME, VERSION_MAJOR, VERSION_MINOR );
		s_gamestate = GAME_OFF;
		s_context = new Context( APP_COUNT );

		//each factory self registers with asset system
	    s_factories[0] = new TilesetFactory();
	    s_factories[1] = new MapFactory();
	    s_factories[2] = new FontFactory();
	    s_factories[3] = new ScriptFactory();
	    s_factories[4] = new GameFactory();

		//default config
		s_sysPath = FS::ExePath() + "/system/";
		s_sysAssetPath = s_sysPath + "assets/";


		Engine::Startup( title, SCREEN_W, SCREEN_H, WINDOW_SCALE );
		Assets::Startup( Sys::AssetPath() );

		s_menu = new MainMenu();
		s_menu->onEnter();

		//setup context apps
		s_context->create( APP_SHELL,          s_shell = new Shell() );
		s_context->create( APP_TILESET_EDITOR, new TilesetEditor()   );
		s_context->create( APP_MAP_EDITOR,     new MapEditor()       );
		s_context->create( APP_SCRIPT_EDITOR,  new ScriptEditor()    );
		s_context->create( APP_GAME_EDITOR,    new GameEditor()    );
		s_shell->addCommands( cmds );

		//redirect key event to appropriate app. 
		//handle system level keys. i.e escape and quick shell toggle (tilda ? )
		Engine::SetKeyEcho( true );
		Engine::PushKeyHandler(
			[&] ( Key key, ButtonState state)
			{
				if ( state == BUTTON_CLICK  )
				{	
					switch ( key )
					{
					case KEY_ESCAPE:
						RunShell( "" );
						return;
					}
				}
				s_context->handleKey( key, state );
				
			}
		);

		//boot into shell
		RunShell( "" );

		LOG( "System On!\n" );
	}

	void Shutdown()
	{
		s_menu->onExit();
		delete s_menu;

		s_context->clear();
		delete s_context;

		Assets::Shutdown();
		Engine::Shutdown();

		for(int i = 0; i < FACTORY_COUNT; i++)
		{
			delete s_factories[i];
		}
		LOG( "System Off!\n" );
		exit( 1 );
	}

	static void EditorStep()
	{
		s_menu->update();

		//run editor context
		switch ( Sys::GetContext()->run() )
		{
		case APP_CODE_CONTINUE:
			break;
		case APP_CODE_SHUTDOWN:
			Sys::Shutdown();
			break;
		case APP_CODE_EXIT:
			s_context->exit();
			break;
		}

		s_menu->draw();
	}


	static void GameStep()
	{
		Idolon::Draw(); 
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

	
	MainMenu *GetMainMenu()
	{
		return s_menu;
	}
	
	
	void RunShell( const std::string &path )
	{
		FS::Cd( path );
		s_menu->hide( true );
		s_context->enter(APP_SHELL, true);

	}

	void RunFontEditor( const std::string &tilesetName )
	{
		s_menu->hide( false );
		//if == "" create 
		s_context->app<TilesetEditor>(APP_TILESET_EDITOR)->setTileset(tilesetName, true );
		s_context->enter(APP_TILESET_EDITOR, false);
	}
	
	void RunTilesetEditor( const std::string &tilesetName )
	{
		s_menu->hide( false );
		s_context->app<TilesetEditor>(APP_TILESET_EDITOR)->setTileset(tilesetName, false );
		s_context->enter(APP_TILESET_EDITOR, false);
	}
	
	void RunMapEditor( const std::string &mapName )
	{
		s_menu->hide( false );
		s_context->app<MapEditor>(APP_MAP_EDITOR)->setMap(mapName);
		s_context->enter(APP_MAP_EDITOR, false);
	}
	
	void RunScriptEditor(const std::string & scriptName )
	{
		s_menu->hide( false );
		s_context->app<ScriptEditor>(APP_SCRIPT_EDITOR)->setScript(scriptName);
		s_context->enter(APP_SCRIPT_EDITOR, false);
	}
		
	void RunGameEditor(const std::string & cartPath )
	{
		s_menu->hide( false );
		s_context->app<GameEditor>(APP_GAME_EDITOR)->setGame(cartPath);
		s_context->enter(APP_GAME_EDITOR, false);
	}

	void RunGame(const std::string & gameName)
	{
		s_menu->hide( true );
		TypedArg ret;
		Game::Startup(gameName);
	}
} // namespace Sys