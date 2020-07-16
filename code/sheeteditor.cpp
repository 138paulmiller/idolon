#include "pch.h"
#include "sheeteditor.h"
#include <iostream>

#define MAX_REVISION_COUNT 100
#define TILE_SIZE_COUNT 4
#define TILE_SIZE_MAX 16

const int s_tileSizes[TILE_SIZE_COUNT][2] = {
	{TILE_W_SMALL, TILE_H_SMALL },
	{TILE_W, TILE_H }
};

using namespace Graphics;
using namespace UI;


SheetEditor::SheetEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{}

void SheetEditor::onEnter()
{

	printf("Entering sheet editor ... ");
	m_usingTool = 0;
	m_sheet = 0;
	m_overlay = 0;
	m_revision = -1;
	m_sheet = Assets::Load<Graphics::Sheet>(m_sheetName);
	
	
	m_charW = Sys::GetShell()->getFont()->charW;
	m_charH = Sys::GetShell()->getFont()->charH;

	int w, h;
	Engine::GetSize(w, h);

	const int x = w - 16 - 8;
	const int y = 8 * 2;

	m_colorPicker = new ColorPicker(x,y);
	m_sheetPicker = new SheetPicker( m_sheet );
	m_sheetPicker->resizeCursor( s_tileSizes[0][0], s_tileSizes[0][1] );

	m_toolbar = new Toolbar(this, 0, m_sheetPicker->rect().y - m_charH);

	m_toolbar->add("PIXEL", [&](){
		m_tool = TOOL_PIXEL;                     
	});
	
	m_toolbar->add("FILL", [&](){
		m_tool = TOOL_FILL;             
	});

	m_toolbar->add("LINE", [&](){
		m_tool = TOOL_LINE;
		m_shapeRect = { -1, -1, -1, -1 };
	});

	m_toolbar->add("ERASE", [&](){
		m_tool = TOOL_ERASE;
	});

	//first add toolbat	
	//add the ui widgets
	App::addWidget( m_toolbar );
	App::addWidget( m_sheetPicker );
	App::addWidget( m_colorPicker );

	//choose pixel tool on start
	m_toolbar->get(TOOL_PIXEL)->click();
	

	if ( m_sheet )
	{
		m_sheet->update();
		commit();
	}
	Editor::onEnter();
}

void SheetEditor::onExit()
{	//allow for reloading data
	Assets::Unload(m_sheetName );
	m_sheetName = "";
	//remove all ui Widgets/buttons
	App::clear();
	for(Color * colors : m_revisionData)
	{
		if(colors)
			delete colors;
	}
	m_revisionData.clear();

	delete m_overlay;
	m_overlay = 0;

	Editor::onExit();
	printf("Extited sheet editor");

}



void SheetEditor::onTick()
{
	//set pixel tool by default if not set
	Engine::ClearScreen(EDITOR_COLOR);
	//update 
	int mx, my;
	Engine::GetMousePosition(mx, my);
	const Color &color = m_colorPicker->color();
	
	//tile position in sheet
	const Rect& tileSrc = m_sheetPicker->selection();
	//if using smal tile size. make pixels larger! 
	
	const float scale = ( TILE_SIZE_MAX / tileSrc.w ) * m_tileScale;
	
	//canvas is the tile drawing region in worldspace 
	const Rect & canvasRect = { m_charW, m_charH * 2, (int)(tileSrc.w * scale), (int)(tileSrc.h * scale) }; 	

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
		case TOOL_LINE:	

			//invalidate
			if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_UP)
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
		
		if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_DOWN )
		{
			switch(m_tool)
			{
			case TOOL_FILL:
				FloodFill(m_sheet->pixels, m_sheet->w, tileSrc, color, sheetx, sheety);
				commit();
				break;
			case TOOL_ERASE:
				m_sheet->pixels[sheety * m_sheet->w + sheetx] = CLEAR;
				m_sheet->update(m_sheetPicker->selection());
				break;
			case TOOL_PIXEL:
				m_sheet->pixels[sheety * m_sheet->w + sheetx] = color;
				m_sheet->update(m_sheetPicker->selection());
				break;
			case TOOL_LINE:
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
			case TOOL_ERASE:
				m_sheet->pixels[sheety * m_sheet->w + sheetx] = CLEAR;
				m_sheet->update(m_sheetPicker->selection());
				break;
			case TOOL_PIXEL:
				m_sheet->pixels[sheety * m_sheet->w + sheetx] = color;
				m_sheet->update(m_sheetPicker->selection());
				break;
			case TOOL_LINE:
				//set shape end (x,y)
				m_shapeRect.w = tilex;
				m_shapeRect.h = tiley;
				break;
			default:
				break;
			}
		}
		else if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_UP)
		{
			if(m_usingTool)
			{
				//commit shape 
				switch(m_tool)
				{
				case TOOL_ERASE:
					commit();
					break;
				case TOOL_PIXEL:
					commit();
					break;
				case TOOL_LINE:
				{
					//if valid
					if ( m_shapeRect.x == -1 ) break;
					const int x1 = tileSrc.x + m_shapeRect.x;
					const int y1 = tileSrc.y + m_shapeRect.y;
					const int x2 = tileSrc.x + m_shapeRect.w;
					const int y2 = tileSrc.y + m_shapeRect.h;
					LineBresenham( m_sheet->pixels, m_sheet->w, x1, y1, x2, y2, color );
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
	Engine::DrawTexture(m_sheet->texture, tileSrc, canvasRect);
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


void SheetEditor::drawOverlay(int tilex, int tiley, const Rect & dest)
{
	const Color &color = m_colorPicker->color();
	const Rect & overlaySrc = { 0, 0, m_sheetPicker->selection().w, m_sheetPicker->selection().h }; 	
	//not valid
	if(tilex < 0  || tilex >= overlaySrc.w || tiley < 0  || tiley >= overlaySrc.h)
		return;
	//local space of canvas in pixels
	//get new overlay if selection size changes
	if(m_overlay == 0 || m_overlay->w != overlaySrc.w || m_overlay->h != overlaySrc.h)
	{
		if(m_overlay == 0)
			delete m_overlay;
		m_overlay = new Graphics::Sheet("SheetEditor_Overlay", overlaySrc.w, overlaySrc.h);
	}

	memset(m_overlay->pixels, 0,  overlaySrc.w * overlaySrc.h * sizeof(Color));
	switch(m_tool)
	{
	case TOOL_LINE:
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
	case TOOL_ERASE:
		//rudimentary "saturate" color to indicate highlight by rendering low alpha white over it
		m_overlay->pixels[tiley * m_overlay->w + tilex] = HIGHLIGHT ;
		break;

	default:
		m_overlay->pixels[tiley * m_overlay->w + tilex] = color;
		break;
	}

	//update every odd  frame ? 
	m_overlay->update();
	Engine::DrawTexture(m_overlay->texture, overlaySrc, dest);

}

//
void SheetEditor::onKey(Key key, bool isDown)
{
	if(isDown)
	{
		switch(key)
		{
			case KEY_UP:
				m_sheetPicker->moveCursor(0, -1);
				break;
			case KEY_DOWN:
				m_sheetPicker->moveCursor(0, 1);
				break;
			case KEY_RIGHT:
				m_sheetPicker->moveCursor(1, 0);
				break;
			case KEY_LEFT:
				m_sheetPicker->moveCursor(-1, 0);
				break;
			case KEY_1:
				m_sheetPicker->resizeCursor( s_tileSizes[0][0], s_tileSizes[0][1] );
				break;
			case KEY_2:
				m_sheetPicker->resizeCursor( s_tileSizes[1][0], s_tileSizes[1][1] );
				break;
			default:
				break;
		}
	}
}


//Create buttons to resize?
void SheetEditor::setSheet(const std::string& name)
{
	printf("Loading sheet %s ...\n", name.c_str());
	m_sheetName = name;
}

void SheetEditor::commit()
{	
	const Rect & selection = m_sheetPicker->selection();
	m_sheet->update(selection);
	
	int size = m_sheet->w * m_sheet->h;

	Color * colors = new Color [size];
	memcpy(colors, m_sheet->pixels, size * sizeof(Color)); 

/*	
	TODO:
	const int sheetIndex = m_sheetPicker->selectionIndex();
	auto it = m_revisions.find(sheetIndex);
	if(it == m_revisions.end())
		m_revisions[sheetIndex] = -1;
	int revisionId = ++m_revisions[sheetIndex];

*/
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
		const int sheetw = m_sheet->w;
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

void SheetEditor::undo()
{ 	
	const Rect & selection = m_sheetPicker->selection();
	const int & sheetIndex = m_sheetPicker->selectionIndex();
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
		int size = m_sheet->w * m_sheet->h;
		const int sx = selection.x;
		const int sy = selection.y;
		const int sheetw = m_sheet->w;
		
		for(int y = 0; y< selection.h; y++)
		{
			for(int x = 0; x< selection.w; x++)
			{
				i = (y+sy) * sheetw + x+sx;
				m_sheet->pixels[i] = m_revisionData[m_revision][ i ];
			}
		}
		m_sheet->update();
	}
}

void SheetEditor::redo()
{
	const Rect & selection = m_sheetPicker->selection();
	const int & sheetIndex = m_sheetPicker->selectionIndex();
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
		int size = m_sheet->w * m_sheet->h;
		const int sx = selection.x;
		const int sy = selection.y;
		const int sheetw = m_sheet->w;

		for(int y = 0; y< selection.h; y++)
		{
			for(int x = 0; x< selection.w; x++)
			{
				i = (y+sy) * sheetw + x+sx;
				m_sheet->pixels[i] = m_revisionData[m_revision][ i ];
			}
		}
		m_sheet->update();
	}

}

void SheetEditor::save()
{
	if(!m_sheet) return;

	m_sheet->update();
	Assets::Save(m_sheet);
}