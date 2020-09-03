#include "game.hpp"
#include "assets/api.hpp"

namespace 
{
	//TODO Sprite Manager should use octree
	struct SpriteManager
	{
		SpriteManager()
		{
			tileset = "";
		}
		~SpriteManager()
		{
			for(Graphics::Sprite * sprite : sprites)
			{
				delete sprite;
			}
			sprites.clear();
		}
		int  spawn(int tileId, int x, int y)
		{
			const int spriteId = sprites.size();
			Graphics::Sprite * sprite = new Graphics::Sprite(tileId, x,y);
			sprite->tileset = tileset;
			sprite->reload();
			sprites.push_back(sprite);
			return spriteId;
		}
		Graphics::Sprite * sprite(int id)
		{
			if(id > -1 && id < sprites.size())
			{
				return  sprites[id];
			}
			return 0;			
		}

		void draw()
		{
			for(Graphics::Sprite * sprite : sprites)
			{
				sprite->draw();
			}
		}

		std::vector<Graphics::Sprite*> sprites;
		//sprite sheet
		std::string tileset;
	} *s_sm;

	Graphics::Map * s_maps[LAYER_COUNT];

	bool s_mapsEnabled[LAYER_COUNT];
	Game::Cartridge * m_cart;
}


//use MMU 


namespace Game
{


	Desc::Desc( const std::string name )
		:Asset(name)
	{
	}

	Cartridge::Cartridge( const std::string & name, const Desc * desc, char * data )
		: Asset(name)
		, m_desc(desc)
		, m_data(data)
	{
		//load header offset
	}

	Cartridge::~Cartridge()
	{

	}	

	Graphics::Map * Cartridge::LoadMap(const std::string & mapname)
	{
		const uint offset = header.offsets[mapname];
		std::istringstream iss;
		iss.str(m_data+offset);

		//load map from cartridge 
		MapFactory * factory = new MapFactory();
		Graphics::Map * map = dynamic_cast<Graphics::Map*>(factory->deserialize(iss));
		delete factory;
		return map;
	}
///////////////////////////////////////////////////////////////////////////////

	void Startup(const std::string & cartpath)
	{
		//load cart asset 
		//load cartridge header and data.
		m_cart = 0;

		s_sm = new SpriteManager();
		
		for(int i = 0 ; i < LAYER_COUNT; i++)
		{
			s_mapsEnabled[i] = 0;
			s_maps[i] = 0;
		}
		//load cart
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
	
	//package game into cartridge
	void Package( const std::string &descname, const std::string &cartpath )
	{
		ASSERT( 0, "Package not implemented" );
	}


	void Unload(Layer layer)
	{

	}

	void Resize(Layer layer, int w, int h)
	{

	}

	void Scroll(Layer layer, int x, int y)
	{

	}
	
	//Sprite Manager 
	//spawn sprite at x y



	void UseSpriteSheet(const std::string & tileset)
	{
	}
}