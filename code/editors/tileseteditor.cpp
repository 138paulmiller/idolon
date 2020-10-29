
#include "../sys.hpp"
#include "tileseteditor.hpp"
#include <iostream>

#define MAX_REVISION_COUNT 100
#define TILE_SIZE_COUNT 4
#define TILE_SIZE_MAX 16


TilesetEditor::TilesetEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{}

void TilesetEditor::onEnter()
{

	LOG("Entering sheet editor ... ");
	Editor::onEnter();

	m_usingTool = 0;
	m_tileset = 0;
	m_overlay = 0;
	m_revision = -1;

	if ( m_isFont )
	{
		m_tileset  = Assets::Load<Graphics::Font>(m_tilesetName);
	}
	else
	{
		m_tileset = Assets::Load<Graphics::Tileset>(m_tilesetName);
	}
	

	const std::string &fontName = DEFAULT_FONT;
	Graphics::Font * font = Assets::Load<Graphics::Font>(fontName);
	m_charW = font->charW;
	m_charH = font->charH;

	int w, h;
	Engine::GetSize(w, h);

	const int x = 8;
	const int y = 16;
	m_colorpicker = new UI::ColorPicker(x,y);
	m_tilepicker = new UI::TilePicker();
	m_tilepicker->reload(m_tileset );


	addTool("PENCIL", [&](){
		m_tool = TILE_TOOL_PENCIL;                     
	});
	
	addTool("FILL", [&](){
		m_tool = TILE_TOOL_FILL;             
	});

	addTool("LINE", [&](){
		m_tool = TILE_TOOL_LINE;
		m_shapeRect = { -1, -1, -1, -1 };
	});

	addTool("ERASER", [&](){
		m_tool = TILE_TOOL_ERASER;
		m_shapeRect = { -1, -1, 1, 1 };

	});

	//first add toolbat	
	//add the ui widgets
	App::addWidget( m_tilepicker );
	App::addWidget( m_colorpicker );

	//choose pixel tool on start
	m_tool = TILE_TOOL_PENCIL;                     

	if ( m_tileset )
	{
		m_tileset->update();
		commit();
	}
}

void TilesetEditor::onExit()
{
	Editor::onExit();
	//remove all ui Widgets/buttons. do not manually delete
	for(Color * colors : m_revisionData)
	{
		if(colors)
			delete colors;
	}
	m_revisionData.clear();

	delete m_overlay;
	m_overlay = 0;

	if ( m_isFont )
	{
		Assets::Unload<Graphics::Font>(m_tilesetName );
	}
	else 
	{
		Assets::Unload<Graphics::Tileset>(m_tilesetName );
	}

	LOG("Exited sheet editor");

}



void TilesetEditor::onTick()
{
	//set pixel tool by default if not set
	Engine::Clear(EDITOR_COLOR);
	if ( !m_tileset ) return;
	//update 
	int mx, my;
	Engine::GetMousePosition(mx, my);
	const Color &color = m_colorpicker->color();
	
	//tile position in sheet
	const Rect& tileSrc = m_tilepicker->selection();
	//if using smal tile size. make pixels larger! 
	
	const float scale = tileSrc.w ? ( TILE_SIZE_MAX / tileSrc.w ) * m_tileScale : m_tileScale;
	
	//canvas is the tile drawing region in worldspace 
	const Rect & canvasRect = { 24, 16, (int)(tileSrc.w * scale), (int)(tileSrc.h * scale) }; 	

	const float mtilex = (mx-canvasRect.x);
	const float mtiley = (my-canvasRect.y);
	
	bool mouseInCanvas = !(mtilex < 0.0f || mtilex >= canvasRect.w || mtiley < 0.0f || mtiley >= canvasRect.h);

	//tile x y of mouse
	const int tilex = mtilex/scale;
	const int tiley = mtiley/scale;
	
	if (!mouseInCanvas)
	{
		switch ( m_tool )
		{
		case TILE_TOOL_LINE:	

			//invalidate
			if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_RELEASE)
				m_shapeRect = { -1, -1, -1, -1 };
			break;
		default:
			break;
		}
	}
	//mouse in bounds!
	else
	{
		//xy in sheet
		int sheetx = tilex + tileSrc.x;
		int sheety = tiley + tileSrc.y;
		
		if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_CLICK )
		{
			switch(m_tool)
			{
			case TILE_TOOL_FILL:
				FloodFill( m_tileset->pixels, m_tileset->w, { tileSrc.x, tileSrc.y, tileSrc.w, tileSrc.h }, color, sheetx, sheety );
				commit();
				break;
			case TILE_TOOL_ERASER:
				m_shapeRect.x = tilex;
				m_shapeRect.y = tiley;
				m_tileset->pixels[sheety * m_tileset->w + sheetx] = CLEAR;
				m_tileset->update(tileSrc);
				break;
			case TILE_TOOL_PENCIL:
				m_tileset->pixels[sheety * m_tileset->w + sheetx] = color;
				m_tileset->update(tileSrc);
				break;
			case TILE_TOOL_LINE:
				//set shape origin (x,y) and dest
				m_shapeRect.x = tilex;
				m_shapeRect.y = tiley;
				m_shapeRect.w = tilex;
				m_shapeRect.h = tiley;
				break;
			default: 
				break;
			}
			m_usingTool = 1;
		}
		else if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD )
		{
			switch ( m_tool )
			{
			case TILE_TOOL_ERASER:

				m_shapeRect.x = tilex;
				m_shapeRect.y = tiley;

				for(int y = 0; y < m_shapeRect.h; y++)
				{
					const int sy = sheety + y;	
					if(sy < tileSrc.y + tileSrc.h)
						for(int x = 0; x < m_shapeRect.w; x++)
						{
							const int sx = sheetx + x;
							if(sx < tileSrc.x + tileSrc.w)
								m_tileset->pixels[sy * m_tileset->w + sx] = CLEAR;
						}
				}
				m_tileset->update(m_tilepicker->selection());
				break;
			case TILE_TOOL_PENCIL:
				m_tileset->pixels[sheety * m_tileset->w + sheetx] = color;
				m_tileset->update(m_tilepicker->selection());
				break;
			case TILE_TOOL_LINE:
				//set shape end (x,y)
				m_shapeRect.w = tilex;
				m_shapeRect.h = tiley;
				break;
			default:
				break;
			}
		}
		else if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_RELEASE)
		{
			if(m_usingTool)
			{
				//commit shape 
				switch(m_tool)
				{
				case TILE_TOOL_ERASER:
					m_shapeRect.x = tilex;
					m_shapeRect.y = tiley;
					commit();
					break;
				case TILE_TOOL_PENCIL:
					commit();
					break;
				case TILE_TOOL_LINE:
				{
					//if valid
					if ( m_shapeRect.x == -1 ) break;
					const int x1 = tileSrc.x + m_shapeRect.x;
					const int y1 = tileSrc.y + m_shapeRect.y;
					const int x2 = tileSrc.x + m_shapeRect.w;
					const int y2 = tileSrc.y + m_shapeRect.h;
					LineBresenham( m_tileset->pixels, m_tileset->w, x1, y1, x2, y2, color );
					//commit to revision stack 
					commit();
					m_shapeRect = { -1, -1, -1, -1 };

					break;
				}
				default:
					break;
				}
			}
			m_usingTool = 0;
				
		}	
	}	

	// draw tile and border
	Engine::DrawTexture(m_tileset->texture, tileSrc, canvasRect);

	const Rect& border = {
			canvasRect.x - 1,
			canvasRect.y - 1,
			canvasRect.w + 2,
			canvasRect.h + 2
	};

	Engine::DrawRect(BORDER_COLOR, border, false);


	if(mouseInCanvas)
		drawOverlay(tilex, tiley, canvasRect);
}


void TilesetEditor::drawOverlay(int tilex, int tiley, const Rect & dest)
{
	const Color &color = m_colorpicker->color();
	const Rect & overlaySrc = { 0, 0, m_tilepicker->selection().w, m_tilepicker->selection().h }; 	
	//not valid
	if(tilex < 0  || tilex >= overlaySrc.w || tiley < 0  || tiley >= overlaySrc.h)
		return;
	//local space of canvas in pixels
	//get new overlay if selection size changes
	if(m_overlay == 0 || m_overlay->w != overlaySrc.w || m_overlay->h != overlaySrc.h)
	{
		if(m_overlay == 0)
			delete m_overlay;
		m_overlay = new Graphics::Tileset("MapEditor_Overlay", overlaySrc.w, overlaySrc.h);
	}

	memset(m_overlay->pixels, 0,  overlaySrc.w * overlaySrc.h * sizeof(Color));
	switch(m_tool)
	{
	case TILE_TOOL_LINE:
		//if not selected
		if ( m_shapeRect.x == -1 )
		{
			//draw clear color if empty, otherwise nothing will display since clear has 0 alpha
			m_overlay->pixels[tiley * m_overlay->w + tilex] = color;
		}
		else
		{
			const int x1 = m_shapeRect.x;
			const int y1 = m_shapeRect.y;
			const int x2 = m_shapeRect.w;
			const int y2 = m_shapeRect.h;
			LineBresenham(m_overlay->pixels, m_overlay->w, x1, y1, x2, y2, color);
		}
		break;
	case TILE_TOOL_ERASER:
		for(int y = tiley; y < tiley + m_shapeRect.h; y++)
		{
			if(y < m_overlay->h)
				for(int x = tilex; x < tilex + m_shapeRect.w; x++)
				{
					if(x < m_overlay->w)
					m_overlay->pixels[y * m_overlay->w + x] = HIGHLIGHT;
				}
		}
		//draw overlay frame ? 
		break;

	default:
		m_overlay->pixels[tiley * m_overlay->w + tilex] = color;
		break;
	}
 
	m_overlay->update();
	Engine::DrawTexture(m_overlay->texture, overlaySrc, dest);

}

//
void TilesetEditor::onKey(Key key, ButtonState state)
{
	if ( m_tilepicker->handleKey( key, state ) ) return;
	if(state == BUTTON_CLICK)
	{
		switch(key)
		{
			case KEY_z:
				if(m_tool == TILE_TOOL_ERASER)
				{
					if(m_shapeRect.w > 1 )
					{
						m_shapeRect.w--;
						m_shapeRect.h--;
					}
				}
				break;
			case KEY_x:
				if(m_tool == TILE_TOOL_ERASER)
				{
					if(m_shapeRect.w < m_tilepicker->selection().w)
					{
						m_shapeRect.w++;
						m_shapeRect.h++;
					}
				}
				break;
			default:
				break;
		}
	}
}


//Create buttons to resize?
void TilesetEditor::setTileset(const std::string& name, bool isFont )
{
	LOG("Loading tileset %s ...\n", name.c_str());
	m_tilesetName = name;
	m_isFont = isFont;
}

void TilesetEditor::commit()
{	
	const Rect & selection = m_tilepicker->selection();
	m_tileset->update(selection);
	
	int size = m_tileset->w * m_tileset->h;

	Color * colors = new Color [size];
	memcpy(colors, m_tileset->pixels, size * sizeof(Color)); 

	m_revision++;
	if(m_revision >= m_revisionData.size())
	{
		m_revisionData.push_back(colors);
	}
	else
	{
		int i;
		const int sx = selection.x;
		const int sy = selection.y;
		const int sheetw = m_tileset->w;
		//assign only selection
		for(int y = 0; y< selection.h; y++)
		{
			for(int x = 0; x< selection.w; x++)
			{
				i = (y+sy) * sheetw + x+sx;
				m_revisionData[m_revision][ i ] = colors[i];
			}
		}	
		//clear the future
		for(int i = m_revision+1; i < m_revisionData.size(); i++)
			delete m_revisionData[i];
		
		//very inefficient
		m_revisionData.erase(m_revisionData.begin()+m_revision+1, m_revisionData.end());

	}
	
	if(m_revisionData.size() >= MAX_REVISION_COUNT)
	{
		int delta = m_revisionData.size() - MAX_REVISION_COUNT;
		//very inefficient
		m_revisionData.erase(m_revisionData.begin(), m_revisionData.begin()+delta);
	}

}

void TilesetEditor::onUndo()
{ 	
	const Rect & selection = m_tilepicker->selection();
	/*TODO
	auto it = m_revisions.find(sheetIndex);
	if(it == m_revisions.end())
		return; //does not exist
	int & revisionId = m_revisions[sheetIndex];
	*/
	
	if ( m_revision > 0 )
	{
		m_revision--;
	
		int i;
		int size = m_tileset->w * m_tileset->h;
		const int sx = selection.x;
		const int sy = selection.y;
		const int sheetw = m_tileset->w;
		
		for(int y = 0; y< selection.h; y++)
		{
			for(int x = 0; x< selection.w; x++)
			{
				i = (y+sy) * sheetw + x+sx;
				m_tileset->pixels[i] = m_revisionData[m_revision][ i ];
			}
		}
		m_tileset->update();
	}
}

void TilesetEditor::onRedo()
{
	const Rect & selection = m_tilepicker->selection();
/*TODO 
	auto it = m_revisions.find(sheetIndex);
	if(it == m_revisions.end())
		return; //does not exist
	int & revisionId = m_revisions[sheetIndex];
*/

	if(m_revision < m_revisionData.size()-1)
	{
		m_revision++;
		int i;
		int size = m_tileset->w * m_tileset->h;
		const int sx = selection.x;
		const int sy = selection.y;
		const int sheetw = m_tileset->w;

		for(int y = 0; y< selection.h; y++)
		{
			for(int x = 0; x< selection.w; x++)
			{
				i = (y+sy) * sheetw + x+sx;
				m_tileset->pixels[i] = m_revisionData[m_revision][ i ];
			}
		}
		m_tileset->update();
	}

}

void TilesetEditor::onSave()
{
	if ( !m_tileset ) return;

	m_tileset->update();

	if ( m_isFont )
	{
		Graphics::Font *font = dynamic_cast<Graphics::Font*>( m_tileset );
		Assets::Save( font );
	}
	else 
	{
		Assets::Save( m_tileset );
	}
}