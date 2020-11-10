
#include "../sys.hpp"
#include "configeditor.hpp"
#include <iostream>


ConfigEditor::ConfigEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{}

void ConfigEditor::onEnter()
{
	Editor::onEnter();

	LOG("Entering config editor ... ");
	
	Editor::addTool("BUILD", [this](){
		this->package();
	}, false);
	
	m_header = Sys::GetGameHeader();
	redrawHeaderData();
}

void ConfigEditor::onExit()
{
	m_header = 0;
	Editor::onExit();
	LOG("Exited game editor");
}



void ConfigEditor::onTick()
{
	Engine::Clear(EDITOR_COLOR);
	if ( m_requiresRedraw )
	{
		redrawHeaderData();
	}
	for ( int i = 0; i < m_labels.size(); i++ )
	{
		m_labels[i]->draw();
	}
}

//
void ConfigEditor::onKey(Key key, ButtonState state)
{
}

void ConfigEditor::onUndo()
{ 	
}

void ConfigEditor::onRedo()
{

}

void ConfigEditor::onSave()
{
	Assets::Save<Game::Header>( m_header );
}

void ConfigEditor::setName( const std::string &name )
{
	if ( m_header )
	{
		m_header->name =  name;
	}

}
void ConfigEditor::package()
{
	if ( m_header )
		{
			const std::string path = FS::DirName( m_header->filepath );
			const std::string &cartPath = path + "/" + m_header->name + DEFAULT_CARTRIDGE_EXT;
			Game::Package( *m_header, cartPath );
		}
}

void ConfigEditor::requestRedrawHeader()
{
	m_requiresRedraw = true;

}

void ConfigEditor::AddComboBox( const std::string &labelText, std::vector<std::string> &names, int x, int y )
{
	const int colx = 75;
	Graphics::TextBox *label = new Graphics::TextBox( labelText.size(), 1, labelText, DEFAULT_FONT );
	label->x = x;
	label->y = y;
	label->textColor = WHITE; 
	label->fillColor = EDITOR_COLOR; 
	label->reload();
	m_labels.push_back( label );
	
	UI::ComboBox * comboBox = new UI::ComboBox( this, x+colx, y, 16, 1 );
	for ( int i = 0; i < names.size(); i++ )
	{
		comboBox->add(names[i]);
	}
	
	comboBox ->cbSelect = [comboBox, &names] ( const std::string & text) { 
		names = comboBox->entries();
	};
	App::addWidget( comboBox  );
}

void ConfigEditor::redrawHeaderData()
{
	if ( m_header == nullptr ) return;

	m_requiresRedraw = false;

	for ( int i = 0; i < m_labels.size(); i++ )
	{
		delete m_labels[i];
	}
	m_labels.clear();
	//TODO add + buttons to add
	// when adding will redraw

	//use just to get w/h
	const std::string &fontName = DEFAULT_FONT;
	Graphics::Font *font = Assets::Load<Graphics::Font>( DEFAULT_FONT );
	const int charW = font->charW;
	const int charH = font->charH;
	Assets::Unload<Graphics::Font>( fontName );

	const std::string &name = m_header->name;

	int inputw = 15;
	int borderx = charW * 2;
	int offy = charH + 4;
	int y = 20;

	//name info
	const std::string &labelText = "Game Name";
	Graphics::TextBox *label = new Graphics::TextBox( labelText.size(), 1, labelText, DEFAULT_FONT );
	label->x = borderx;
	label->y = y;
	label->textColor = WHITE; 
	label->fillColor = EDITOR_COLOR; 
	label->reload();
	m_labels.push_back( label );

	y += offy;
	
	AddComboBox( "Maps", m_header->maps, borderx, y );
	y += offy;
	AddComboBox( "Tilesets", m_header->tilesets, borderx, y);
	y += offy;
	AddComboBox( "Scripts", m_header->scripts, borderx, y);
	y += offy;
	AddComboBox( "Fonts", m_header->fonts, borderx, y);
	y += offy;

}


