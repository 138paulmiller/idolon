

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

	/*
	Memory layout
	Tileset (4096 bytes)
	
	0x00000 - 0x03FFF : Tileset 0	
	0x04000 - 0x07FFF : Tileset 1	
	0x08000 - 0x0BFFF : Tileset 2	
	0x0C000 - 0x0FFFF : Tileset 3
	
	*/

	//create a swap function that loads from cartridge into CPU.
	// the tileset index is specified in the map section of the cartridge, so when loading a map, also load the tileset index
	class CPU
	{
		char * tilesets[LAYER_COUNT];
	};
	
	struct Page //similar to extent window over data
	{
		int offset;
		int size;
	};

	//all asset files in the desc are collated into this single block of memory
	//encode within png ?
	
	union MMU 
	{
		Page * pages;
		int pageCount;
		//todo use mmap
		char * data;
	} mmu;

	//maps from tileset/map name to page. 
	std::unordered_map<char *, Page> s_memmap; 

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
				{	
					s_context->app()->signal( APP_CODE_CONTINUE );
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
		Eval::Test();

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
				//cannot exit shell.
				//set current back to continue so can reenter
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