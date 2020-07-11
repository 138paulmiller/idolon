#include "graphics.h"
#include "engine.h"
#include "assets.h"
#include "sheeteditor.h"
#include "tools.h"

#include <iostream>


using namespace Graphics;
using namespace UI;



void SheetEditor::onEnter()
{

	printf("Entering sheet editor ... ");
	m_sheet = 0;
	m_tool = TOOL_PIXEL;
	m_sheet = Assets::Load<Graphics::Sheet>(m_sheetName);
	
	if(m_sheet)
		m_sheet->update();

	m_sheetPicker = new SheetPicker( m_sheet );
	m_colorPicker = new ColorPicker();
	m_toolbar = new Toolbar(this, 0, m_sheetPicker->rect().y - FONT_H);
	
	//add undo, redo. 
	TextButton * savebutton = new TextButton("Save", 0, 0, 4, 1);
	savebutton->sticky = false;
	savebutton->cbClick = [&](){
		if(!m_sheet) return;
		m_sheet->update();
		Assets::Save(m_sheet);
	};
	//click it
	m_toolbar->add("PIXEL", [&](){
		m_tool = TOOL_PIXEL;                     
	})->onClick();
	
	m_toolbar->add("FILL", [&](){
		m_tool = TOOL_FILL;                     
	});

	m_toolbar->add("LINE", [&](){
		m_tool = TOOL_LINE;  
		m_shapeRect = { -1, -1, -1, -1 };
	});

	
	//add the ui widgets
	App::addButton( savebutton );	
	App::addWidget( m_toolbar );
	App::addWidget( m_sheetPicker );
	App::addWidget( m_colorPicker );
}

void SheetEditor::onExit()
{
	//allow for reloading data
	Assets::Unload(m_sheetName );
	m_sheetName = "";
	//remove all ui Widgets/buttons
	App::clear();
	printf("Extited sheet editor");
}


void SheetEditor::onTick()
{

	Engine::ClearScreen(EDITOR_COLOR);
	//update 
	int mx, my;
	Engine::GetMousePosition(mx, my);
	const Color &color = m_colorPicker->color();
	//draw current tile
	const Rect& tileSrc = m_sheetPicker->selection();
	const Rect & tileDest = { FONT_W, FONT_H * 2, tileSrc.w * m_tileScale, tileSrc.h * m_tileScale }; 	

	//tile x y 
	int tmx = ((mx-tileDest.x) / m_tileScale) * m_tileScale;
	int tmy = ((my-tileDest.y)/ m_tileScale) * m_tileScale;

	if ( m_sheet )
	{
		if (tmx >= 0 && tmx < tileDest.w && tmy >= 0 && tmy < tileDest.h)
		{
			//xy in sheet
			int smx = ( tmx  / m_tileScale) + tileSrc.x;
			int smy = ( tmy  / m_tileScale) + tileSrc.y;
			


			if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_HOLD )
			{
				switch ( m_tool )
				{
				case TOOL_PIXEL:
					m_sheet->pixels[smy * m_sheet->w + smx] = color;
					m_sheet->update(tileSrc);
					break;
				case TOOL_LINE:
					//set shape end (x,y)
					m_shapeRect.w = smx;
					m_shapeRect.h = smy;
					break;
				default:
					break;
				}
			}
			else if ( Engine::GetMouseButtonState( MOUSEBUTTON_LEFT ) == BUTTON_DOWN )
			{
				switch(m_tool)
				{
				case TOOL_FILL:
					FloodFill(m_sheet->pixels, m_sheet->w, tileSrc, color, smx, smy);
					m_sheet->update(tileSrc);
				break;
				case TOOL_PIXEL:
					m_sheet->pixels[smy * m_sheet->w + smx] = color;
					m_sheet->update(tileSrc);
					break;
				case TOOL_LINE:
					//set shape origin (x,y)
					m_shapeRect.x = smx;
					m_shapeRect.y = smy;
					break;
				}

			}
			else if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) == BUTTON_UP)
			{
				//submit shape 
				switch(m_tool)
				{
				case TOOL_LINE:
				{
					const int x1 = m_shapeRect.x;
					const int y1 = m_shapeRect.y;
					const int x2 = m_shapeRect.w;
					const int y2 = m_shapeRect.h;

					LineBresenham( m_sheet->pixels, m_sheet->w, x1, y1, x2, y2, color );
					m_sheet->update(tileSrc);
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

	//draw tool 
	//if line (shift down)
	if (tmx >= 0 && tmx < tileDest.w && tmy >= 0 && tmy < tileDest.h)
		Engine::DrawRect(color, { tileDest.x+tmx, tileDest.y+tmy, m_tileScale, m_tileScale}, true);
	

	switch(m_tool)
	{
	case TOOL_LINE:
	{
		if ( m_shapeRect.x == -1 ) break;
		const int x1 = tileDest.x + m_shapeRect.x * m_tileScale;
		const int y1 = tileDest.y + m_shapeRect.y * m_tileScale;
		const int x2 = tileDest.x + m_shapeRect.w * m_tileScale;
		const int y2 = tileDest.y + m_shapeRect.h * m_tileScale;
		//stash old tile value. reset then redraw with line
		Engine::DrawLine(color, x1, y1, x2, y2);
		break;
	}
	default:
		break;
	}

}

//
void SheetEditor::onKey(Key key, bool isDown)
{

}


//Create buttons to resize?
void SheetEditor::setSheet(const std::string& name)
{
	printf("Loading sheet %s ...\n", name.c_str());
	m_sheetName = name;
}

