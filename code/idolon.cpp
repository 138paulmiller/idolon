#pragma once
#include "idolon.hpp"
#include "game.hpp"


namespace Idolon
{
	//TODO Sprite Manager should use octree
	struct SpriteManager
	{
		SpriteManager()
		{
			tileset = "";
		}

		void clear()
		{
			for(Graphics::Sprite * sprite : sprites)
			{
				delete sprite;
			}
			sprites.clear();
		}
		int  spawn(int tileId, int x, int y, int w, int h)
		{
			const int spriteId = sprites.size();
			Graphics::Sprite * sprite = new Graphics::Sprite(tileId, w,h);
			sprite->x = x;
			sprite->y = y;
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
	} s_sm;

	Graphics::Map * s_maps[LAYER_COUNT];

	bool s_mapsEnabled[LAYER_COUNT];
	Game::Cartridge * m_cart;
}
#define GET_MAP(name, layer, ...)\
	if(layer >= 0 && layer < LAYER_COUNT)\
	{\
		Graphics::Map *name = s_maps[ layer ];\
		if (name)\
		{\
			__VA_ARGS__\
		}\
	}

#define GET_SPRITE(name, spriteId, ...) 		\
	Graphics::Sprite *name = s_sm.sprite( spriteId );\
	if (name)\
	{\
		__VA_ARGS__\
	}


namespace Idolon
{
	void Boot( const char *cartname )
	{
		m_cart = Assets::Load<Game::Cartridge>( cartname );
		//load game code from cartridge.
		s_sm.clear();
	}

	void  Step()
	{
		//handle collisions. dispatch events
		//update game state 
		//handle return state RUN, EXIT, PAUSE
		//clear screen, draw map, draw sprites, draw ui
		Engine::ClearScreen();
		for(int i = 0 ; i < LAYER_COUNT; i++)
		{
			if( s_mapsEnabled[i])
				s_maps[i]->draw();
		}
		s_sm.draw();
	}
	void Quit()
	{
		s_sm.clear();
	}

	// =============================== Sprite ==================================
	
	void Load( int layer, const char *mapname )
	{
		bool success;
		//if no cartridge use assets
		if ( m_cart == 0 )
		{
			Unload( layer );
			s_maps[layer] = Assets::Load<Graphics::Map>( mapname );
		}
		else
		{
			s_maps[layer] = m_cart->LoadMap(mapname);
		}
		//success
		s_mapsEnabled[layer] = ( s_maps[layer] != nullptr );
	}
	
	void Unload( int layer )
	{
		if(m_cart == 0 && s_maps[layer])
			Assets::Unload<Graphics::Map>(s_maps[layer]->name);

		s_mapsEnabled[layer] = 0;
	}
	
	
	//scroll map to x,y
	void Scroll(int layer, int dx, int dy)
	{
		GET_MAP(map, layer,
			map->scroll(dx, dy);
		)
	}

	void View(int layer, int x, int y, int w, int h)
	{
		GET_MAP(map, layer,
			map->view = { x,y,w,h };
			map->clampView();
		)
	}


	int TileAt(int layer, int x, int y)
	{
		GET_MAP(map, layer,
			int tx, ty;
			if (map->getTileXY(x, y, tx, ty))
			{
				return map->tiles[ty * map->w + tx];
			}
		)
		return -1;
	}
	// =============================== Sprite ==================================

	int Spawn(int tileId, int x, int y, bool isSmall)
	{
		if( tileId < 0 || tileId >= (isSmall ? SPRITE_SMALL_COUNT : SPRITE_COUNT )  )
		{
			return -1;
		}
		const int w = ( isSmall ? SPRITE_W_SMALL : SPRITE_W );
		const int h = ( isSmall ? SPRITE_H_SMALL : SPRITE_H );
		return s_sm.spawn(tileId, x, y,w,h);
	}

	void Despawn(int spriteId)
	{
		if(spriteId > -1 && spriteId < s_sm.sprites.size())
		{
			return ;
		}
		delete 	s_sm.sprites[spriteId];
		s_sm.sprites[spriteId] = 0;
	}
	

	void MoveTo( int spriteId, int x, int y )
	{
		GET_SPRITE(sprite, spriteId,
			sprite->x = x;
			sprite->y = y;
		)
	}
	void MoveBy( int spriteId, int dx, int dy )
	{
		GET_SPRITE(sprite, spriteId,
			sprite->x += dx;
			sprite->y += dy;
		)
	}

	void FlipTo( int spriteId, int tileId )
	{
		GET_SPRITE(sprite, spriteId,
			sprite->tile = tileId;
			sprite->tile = Clamp(sprite->tile, 0, (sprite->w == SPRITE_H_SMALL ? SPRITE_SMALL_COUNT : SPRITE_COUNT) );
		)

	}

	void FlipBy( int spriteId, int di )
	{
		GET_SPRITE(sprite, spriteId,
			sprite->tile += di;
			sprite->tile = Clamp(sprite->tile, 0, (sprite->w == SPRITE_H_SMALL ? SPRITE_SMALL_COUNT : SPRITE_COUNT) );
		)
	}

	int Frame( int spriteId )
	{
		GET_SPRITE(sprite, spriteId,
			return sprite->tile;
		)
		return -1;
	}

	bool Position( int spriteId, int & x, int & y)
	{
		GET_SPRITE(sprite, spriteId,
			x = sprite->x;
			y = sprite->y;
			return true;
		)
		return false;
	}

	void Sheet( const char *tileset )
	{
		Assets::Unload<Graphics::Tileset>( tileset );
		s_sm.tileset = tileset;
	}	


}