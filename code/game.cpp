#include "game.hpp"
namespace 
{

	//TODO Sprite Manager should use octree
	struct SpriteManager
	{
		~SpriteManager()
		{
			for(Graphics::Sprite * sprite : sprites)
			{
				delete sprite;
			}
			sprites.clear();
		}

		void draw()
		{
			for(Graphics::Sprite * sprite : sprites)
			{
				sprite->draw();
			}
		}

		std::vector<Graphics::Sprite*> sprites;
	} *s_sm;

	std::string s_spritesheet;
	Graphics::Map * s_maps[LAYER_COUNT];

	bool s_mapsEnabled[LAYER_COUNT];
}


//use MMU 


namespace Game
{

	Cartridge::Cartridge(const Desc & desc )
	{
	}

	Cartridge::~Cartridge()
	{

	}	

	void Startup(const std::string & gameName)
	{
		//load cart asset 
		s_sm = new SpriteManager();
		s_spritesheet = "";
		
		for(int i = 0 ; i < LAYER_COUNT; i++)
		{
			s_mapsEnabled[i] = 0;
			s_maps[i] = 0;
		}
		//load cart
		const std::string sourcefile = ""; 
		//compile game code
//		Eval::Compile(sourcefile);

	}

	//Step the game
	GameState Run()
	{
		//handle collisions. dispatch events
		//update game state 
		//handle return state RUN, EXIT, PAUSE
		//clear screen, draw map, draw sprites, draw ui
		for(int i = 0 ; i < LAYER_COUNT; i++)
		{
			if( s_mapsEnabled[i])
				s_maps[i]->draw();
		}
		s_sm->draw();
		
		return GAME_RUNNING;
	}

	void Shutdown()
	{
		delete s_sm;
		for(int i = 0 ; i < LAYER_COUNT; i++)
		{
			if(s_maps[i])
				delete s_maps[i];
		}

	}

	//load map into layer. unloads previous
	void Load(Layer layer, const std::string & mapname)
	{

		bool success = (s_maps[layer] = Assets::Load<Graphics::Map>(mapname));
		s_mapsEnabled[layer] = success;

	}
	void Unload(Layer layer)
	{
		if(s_maps[layer])
			Assets::Unload<Graphics::Map>(s_maps[layer]->name);

		s_mapsEnabled[layer] = 0;
	}

	void Resize(Layer layer, int x, int y)
	{

	}

	void Scroll(Layer layer, int x, int y)
	{

	}
	
	//Sprite Manager 
	//spawn sprite at x y
	int Spawn(int tileid, int x, int y, bool isSmall)
	{
		if( tileid < 0 || tileid < (isSmall ? SPRITE_SMALL_COUNT : SPRITE_COUNT )  )
		{
			return -1;
		}
		const int id = s_sm->sprites.size();
		Graphics::Sprite * sprite = new Graphics::Sprite(tileid, x,y);
		sprite->tileset = s_spritesheet;
		sprite->reload();
		s_sm->sprites.push_back(sprite);
		return id;
	}
	//despawn sprite
	void Despawn(int spriteId)
	{
		if(spriteId > -1 && spriteId < s_sm->sprites.size())
		{
			return ;
		}
		delete 	s_sm->sprites[spriteId];
		s_sm->sprites[spriteId] = 0;

	}
	Graphics::Sprite * GetSprite(int spriteId)
	{
		ASSERT(s_sm, "Game is shut down!");
		if(spriteId > -1 && spriteId < s_sm->sprites.size())
		{
			return  s_sm->sprites[spriteId];
		}
		return 0;
	}

	void UseSpriteSheet(const std::string & tileset)
	{
		s_spritesheet = tileset;
	}
}