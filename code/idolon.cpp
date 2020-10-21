#pragma once
#include "idolon.hpp"
#include "game.hpp"
#include "sys.hpp"


namespace Idolon
{
	/*
		TODO - all graphics elements should not use Asset::Load/Unload but rather contain weak pointers to assets. All loading Unloading should be handled at this level, to allow cartridge or disk IO
	*/

	//TODO Sprite Manager should use octree
	struct SpriteManager
	{
		SpriteManager()
		{
			tileset = nullptr;
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
			sprite->tileset = tileset ? tileset->name : "";
			sprite->reload();
			sprites.push_back(sprite);
			return spriteId;
		}

		int despawn( int spriteId  )
		{
			if(spriteId > -1 && spriteId < sprites.size())
			{
				return 0;
			}
			delete 	sprites[spriteId];
			sprites[spriteId] = 0;
			return 1;
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
		Graphics::Tileset * tileset;
	} s_sm;

	//these are owning pointers
	std::vector<Asset *> s_assets;
	//these are weak ref
	Graphics::Map* s_maps[LAYER_COUNT];

	bool s_mapsEnabled[LAYER_COUNT];
	Game::Cartridge * m_cart;

	//draw mode 
	int s_drawMode = DRAWMODE_TILED;

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

	void Draw()
	{
		//if tiled mode
		if ( s_drawMode == DRAWMODE_TILED )
		{
			//handle collisions. dispatch events
			//update game state 
			//handle return state RUN, EXIT, PAUSE
			//clear screen, draw map, draw sprites, draw ui
			Engine::Clear();
			for(int i = 0 ; i < LAYER_COUNT; i++)
			{
				if( s_mapsEnabled[i])
					s_maps[i]->draw();
			}
			s_sm.draw();
		}
	}
	void Quit()
	{
		s_sm.clear();
		for ( int i = 0; i < s_assets.size(); i++ )
		{
			Unload( i );
		}
		for ( int i = 0; i < LAYER_COUNT; i++ )
		{
			s_maps[i] = 0;
		}
	}

	bool Import( const char *filepath )
	{
		//tries system lib
		return Eval::Import( filepath );
	}

	template <typename AssetType>
	inline AssetType* LoadAs( const std::string & assetName )
	{
		if ( m_cart == 0 )
		{
			return Assets::Load<AssetType>( assetName );
		}
		else
		{
			return m_cart->Load<AssetType>( assetName );
		}
	}
	
	
	template <typename AssetType>
	inline void UnloadAs( AssetType *asset )
	{
		if ( m_cart == 0 )
		{
			if( asset )
				Assets::Unload<AssetType>( asset->name );
		}
		else
		{
			m_cart->Unload<AssetType>( asset);
		}
	}

	int Load( const char *assetName )
	{
		int id ;
		//find empty or end
		for ( id = 0; id < s_assets.size(); id++ )
		{
			if ( s_assets[id] == 0 )
			{
				break;
			}
		}
		if ( id == s_assets.size() )
		{
			s_assets.emplace_back();
		}
		const std::string & type = FS::FileExt( assetName );
		const std::string& name = FS::NoExt(assetName);

		//add 
		if ( type == "map" )
		{
			s_assets[id] = LoadAs<Graphics::Map>( name );
		}
		else if ( type == "tls" )
		{
			s_assets[id] = LoadAs<Graphics::Tileset>( name );
		}
		else if ( type == "scr" )
		{
			s_assets[id] = LoadAs<Script>( name );
		}
		else
		{
			LOG( "Load:  Could not load %s %s", name.c_str(), type.c_str() );
		}
		//if valid 
		if ( !s_assets[id] )
		{
			id = -1;
		}
		return id;	

	}

	void Unload( int assetId )
	{
		//add 
		if ( assetId >= 0 && assetId < s_assets.size() )
		{
			Asset *asset = s_assets[assetId];
			if ( asset )
			{
				const std::string & type = FS::FileExt( asset->name );
				if ( type == "map" )
				{
					UnloadAs( dynamic_cast< Graphics::Map *>(asset) );
				}
				else if ( type == "tls" )
				{
					UnloadAs( dynamic_cast< Graphics::Tileset *>(asset) );
				}
				else if ( type == "scr" )
				{
					UnloadAs( dynamic_cast< Script*>(asset) );
				}
				s_assets[assetId] = 0;
			}
		}
	}

	void MousePosition( int &x, int &y )
	{
		Engine::GetMousePosition( x, y);
	}
	
	int GetKeyState( char keysym )
	{
		Key key = static_cast<Key>(keysym);
		const int state = Engine::GetKeyState(key);
		return state;
	}
	

	void GfxSize( int &w, int &h )
	{
		Engine::GetSize(w, h);
	}
	
	void GfxDrawMode( int mode  )
	{
		s_drawMode = mode;
	}



	// ==== Primitive Drawing api === 
	// clear the display
	void GfxClear( unsigned char r, unsigned char g, unsigned char b )
	{
		const Color & c = { 255,r,g,b };
		Engine::Clear(c);	
	}

	int GfxCreateTexture( int width, int height )
	{
		return Engine::CreateTexture( width, height, TEXTURE_STATIC );
	}

	void GfxDestroyTexture( int textureId )
	{
		Engine::DestroyTexture(textureId );
	}
	
	void GfxUpdateTexture( int textureId, const unsigned char *pixels, int width, int x, int y, int w, int h )
	{
		const Rect &rect = { x,y,w,h };
		Engine::UpdateTexture( textureId, reinterpret_cast<const Color*>(pixels), width, rect );
	}
    
	void GfxDrawTexture( int textureId, 		
		int sx, int sy, int sw, int sh, 
		int dx, int dy, int dw, int dh 
	)
	{
		const Rect &src = { sx,sy,sw,sh };
		const Rect &dst= { dx,dy,dw,dh };

		Engine::DrawTexture( textureId, src, dst );
	}

	void GfxDrawLine( 
		unsigned char r, unsigned char g, unsigned char b, unsigned char a, 
		int x1, int y1, int x2, int y2 
	)
	{
		const Color &color = { a,r,g,b };
		Engine::DrawLine( color, x1, y1, x2, y2  );
	}

	void GfxDrawRect(
		unsigned char r, unsigned char g, unsigned char b, unsigned char a,  
		int x, int y, int w, int h, bool filled 
	)
	{
		const Color &color = { a,r,g,b };
		const Rect &rect = { x,y,w,h };
		Engine::DrawRect( color, rect, filled );
	}
	
	// =============================== Map ==================================
	
	void MapLoad( int layer, int mapId )
	{
		bool success;
		//if no cartridge use assets
		s_mapsEnabled[layer] = 0;
		if ( mapId >= 0 && mapId < s_assets.size() )
		{
			s_maps[layer] = dynamic_cast<Graphics::Map*>(s_assets[mapId]);
		}
		else
		{
			s_maps[layer] = 0;
		}
		//success
		s_mapsEnabled[layer] = ( s_maps[layer] != nullptr );
	}
	
	
	//scroll map to x,y
	void MapScrollTo(int layer, int x, int y)
	{
		if(layer >= 0 && layer < LAYER_COUNT)
		{
			Graphics::Map *map = s_maps[ layer ];
			if (map)
			{
				const int dx = x - map->view.x;
				const int dy = y - map->view.y;
				map->scroll(dx, dy);
			}
		}
	}
	//scroll map to x,y
	void MapScrollBy(int layer, int dx, int dy)
	{
		if(layer >= 0 && layer < LAYER_COUNT)
		{
			Graphics::Map *map = s_maps[ layer ];
			if (map)
			{
				map->scroll(dx, dy);
			}
		}
	}

	void MapView(int layer, int x, int y, int w, int h)
	{
		if(layer >= 0 && layer < LAYER_COUNT)
		{
			Graphics::Map *map = s_maps[ layer ];
			if (map)
			{
				map->view = { x,y,w,h };
				map->clampView();
			}
		}
	}

	int MapSetTile( int layer, int x, int y, int tile )
	{
		if(layer >= 0 && layer < LAYER_COUNT)
		{
			Graphics::Map *map = s_maps[ layer ];
			if (map)
			{
				int tilex, tiley;
				if (map->getTileXY(x, y, tilex, tiley))
				{
					map->tiles[tiley * map->w + tilex] = tile;
					map->update(  { tilex, tiley, 1, 1 }  );
				}
			}
		}
		return -1;	
	}
	
	int MapGetTile( int layer, int x, int y )
	{
		if(layer >= 0 && layer < LAYER_COUNT)
		{
			Graphics::Map *map = s_maps[ layer ];
			if (map)
			{
				int tilex, tiley;
				if (map->getTileXY(x, y, tilex, tiley))
				{
					return map->tiles[tiley * map->w + tilex];
				}
			}
		}
		return -1;	
	}

	// =============================== Sprite ==================================

	int SprSpawn(int tileId, int x, int y, bool isSmall)
	{
		if( tileId < 0 || tileId >= (isSmall ? SPRITE_SMALL_COUNT : SPRITE_COUNT )  )
		{
			return -1;
		}
		const int w = ( isSmall ? SPRITE_W_SMALL : SPRITE_W );
		const int h = ( isSmall ? SPRITE_H_SMALL : SPRITE_H );
		return s_sm.spawn(tileId, x, y,w,h);
	}

	void SprDespawn(int spriteId)
	{
		s_sm.despawn(spriteId);
	}
	

	void SprSetPosition( int spriteId, int x, int y )
	{
		GET_SPRITE(sprite, spriteId,
			sprite->x = x;
			sprite->y = y;
		)
	}


	void SprSetFrame( int spriteId, int tileId )
	{
		GET_SPRITE(sprite, spriteId,
			sprite->tile = tileId;
			sprite->tile = Clamp(sprite->tile, 0, (sprite->w == SPRITE_H_SMALL ? SPRITE_SMALL_COUNT : SPRITE_COUNT) );
		)

	}



	int SprGetFrame( int spriteId )
	{
		GET_SPRITE(sprite, spriteId,
			return sprite->tile;
		)
		return -1;
	}

	bool SprGetPosition( int spriteId, int & x, int & y)
	{
		GET_SPRITE(sprite, spriteId,
			x = sprite->x;
			y = sprite->y;
			return true;
		)
		return false;
	}

	
	bool SprGetSize( int spriteId, int & w, int & h)
	{
		GET_SPRITE(sprite, spriteId,
			w = sprite->w;
			h = sprite->h;
			return true;
		)
		return false;
	}

	void SprSetSheet( int tilesetId  )
	{
		if ( tilesetId >= 0 && tilesetId < s_assets.size() )
		{
			s_sm.tileset = dynamic_cast<Graphics::Tileset*>(s_assets[tilesetId]);
		}
	}	


}