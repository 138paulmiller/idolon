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
	
	TextButton * savebutton = new TextButton("Save", 0, 0, 4, 1);
	savebutton->sticky = false;
	savebutton->cbClick = [&](){
		if(!m_sheet) return;
		m_sheet->update();
		Assets::Save(m_sheet);
	};
	
	m_toolbar->add("PIXEL", [&](){
		m_tool = TOOL_PIXEL;                     
	});

	m_toolbar->add("FILL", [&](){
		m_tool = TOOL_FILL;                     
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

	int tmx = ((mx-tileDest.x) / m_tileScale) * m_tileScale;
	int tmy = ((my-tileDest.y)/ m_tileScale) * m_tileScale;

	if (Engine::GetMouseButtonState(MOUSEBUTTON_LEFT) != BUTTON_UP)
	{
		if(!m_sheet) return;

		//get pixel in sheet 
		if (tmx >= 0 && tmx < tileDest.w && tmy >= 0 && tmy < tileDest.h)
		{

			int smx = ( tmx  / m_tileScale) + tileSrc.x;
			int smy = ( tmy  / m_tileScale) + tileSrc.y;
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

	//draw pixel brush
	if (tmx >= 0 && tmx < tileDest.w && tmy >= 0 && tmy < tileDest.h)
		Engine::DrawRect(color, { tileDest.x+tmx, tileDest.y+tmy, m_tileScale, m_tileScale}, true);
	

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

