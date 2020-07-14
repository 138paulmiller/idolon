#include "graphics.h"
#include "engine.h"
#include "assets.h"
#include "sheeteditor.h"
#include "tools.h"

#include <iostream>

#define MAX_REVISION_COUNT 100

using namespace Graphics;
using namespace UI;


SheetEditor::SheetEditor()
	:App(
		SUPPORT( APP_SAVE )
	)
{}

void SheetEditor::onEnter()
{

	printf("Entering sheet editor ... ");
	
	m_sheet = 0;
	m_overlay = 0;
	m_revisionId = 0;
	m_sheet = Assets::Load<Graphics::Sheet>(m_sheetName);
	
	m_sheetPicker = new SheetPicker( m_sheet );
	m_colorPicker = new ColorPicker();

	m_toolbar = new Toolbar(this, 0, m_sheetPicker->rect().y - FONT_H);

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

}

void SheetEditor::onExit()
{	//allow for reloading data
	Assets::Unload(m_sheetName );
	m_sheetName = "";
	//remove all ui Widgets/buttons
	App::clear();
	for(Color * colors : m_revisions)
	{
		if(colors)
			delete colors;
	}
	m_revisions.clear();

	delete m_overlay;
	m_overlay = 0;

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
	//draw current tile
	const Rect& tileSrc = m_sheetPicker->selection();
	const Rect & tileDest = { FONT_W, FONT_H * 2, tileSrc.w * m_tileScale, tileSrc.h * m_tileScale }; 	
	
	//tile x y of mouse
	int tilex = ((mx-tileDest.x) / m_tileScale);
	int tiley = ((my-tileDest.y)/ m_tileScale) ;

	if ( m_sheet )
	{

		//Perhaps each tool could have an interface ratherthan switching ...
		// mouse leave, new mouse pos , with state   
		//if mouse has gone out of boundes
		if (tilex < 0 || tilex >= tileSrc.w || tiley < 0 || tiley >= tileSrc.h)
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
			
			if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD )
			{
				switch ( m_tool )
				{
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
			else if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_DOWN )
			{
				printf( "Down\n" );
				switch(m_tool)
				{
				case TOOL_FILL:
					FloodFill(m_sheet->pixels, m_sheet->w, tileSrc, color, sheetx, sheety);
					commit();
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

			}
			else if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_UP)
			{
				//commit shape 
				switch(m_tool)
				{
				case TOOL_PIXEL:
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
		}	
	}

	// draw tile and border
	Engine::DrawTexture(m_sheet->texture, tileSrc, tileDest);
	const Rect& border = {
			tileDest.x - 1,
			tileDest.y - 1,
			tileDest.w + 2,
			tileDest.h + 2
	};
	Engine::DrawRect(BORDER_COLOR, border, false);

	drawOverlay(tilex, tiley, tileDest);
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
	const Color& cursorColor = (color == COLOR_EMPTY ? EDITOR_COLOR : color);
	switch(m_tool)
	{
	case TOOL_LINE:
		//if not selected
		if ( m_shapeRect.x == -1 )
		{
			//draw clear color if empty, otherwise nothing will display since clear has 0 alpha
			m_overlay->pixels[tiley * m_overlay->w + tilex] = cursorColor;
		}
		else
		{
			const int x1 = m_shapeRect.x;
			const int y1 = m_shapeRect.y;
			const int x2 = m_shapeRect.w;
			const int y2 = m_shapeRect.h;
			LineBresenham(m_overlay->pixels, m_overlay->w, x1, y1, x2, y2, cursorColor);
		}
		break;
	default:
		m_overlay->pixels[tiley * m_overlay->w + tilex] = cursorColor;
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
	printf("COMMIT: %d / %d\n", m_revisionId, m_revisions.size());
	
	m_sheet->update(m_sheetPicker->selection());

	int size = m_sheet->w * m_sheet->h;
	Color * colors = new Color [size];
	memcpy(colors, m_sheet->pixels, size * sizeof(Color)); 
	
	if(m_revisionId >= m_revisions.size())
	{
		m_revisions.push_back(colors);
	}
	else
	{
		m_revisions[m_revisionId] = colors;
		//clear the future
		for(int i = m_revisionId+1; i < m_revisions.size(); i++)
			delete m_revisions[i];
		
		//very inefficient
		m_revisions.erase(m_revisions.begin()+m_revisionId+1, m_revisions.end());

	}
	
	if(m_revisions.size() >= MAX_REVISION_COUNT)
	{
		int delta = m_revisions.size() - MAX_REVISION_COUNT;
		//very inefficient
		m_revisions.erase(m_revisions.begin(), m_revisions.begin()+delta);
	}
	m_revisionId++; 

}

void SheetEditor::undo()
{ 	

	printf("UNDO : %d / %d\n", m_revisionId, m_revisions.size());
	if ( m_revisionId > 0 )
	{
		m_revisionId--;
		int size = m_sheet->w * m_sheet->h;
		memcpy(m_sheet->pixels, m_revisions[m_revisionId], size * sizeof(Color)); 
		m_sheet->update();
	}
}

void SheetEditor::redo()
{
	printf("REDO : %d / %d\n", m_revisionId, m_revisions.size());
	if(m_revisionId < m_revisions.size())
	{
		m_revisionId++;
		int size = m_sheet->w * m_sheet->h;
		memcpy(m_sheet->pixels, m_revisions[m_revisionId], size * sizeof(Color)); 
		m_sheet->update();
	}

}

void SheetEditor::save()
{
	if(!m_sheet) return;
	m_sheet->update();
	Assets::Save(m_sheet);
}