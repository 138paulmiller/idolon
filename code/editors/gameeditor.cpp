
#include "../sys.hpp"
#include "gameeditor.hpp"
#include <iostream>


GameEditor::GameEditor()
	:Editor(
		SUPPORT( APP_SAVE ) |
		SUPPORT( APP_UNDO ) |
		SUPPORT( APP_REDO )  
	)
{}

void GameEditor::onEnter()
{
	Editor::onEnter();

	LOG("Entering game editor ... ");
	int w, h;
	Engine::GetSize(w, h);

	redrawHeaderData();
	
	Editor::addTool("BUILD", [this](){
		this->package();
	}, false);

}

void GameEditor::onExit()
{
	Editor::onExit();
	LOG("Exited game editor");
}



void GameEditor::onTick()
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
void GameEditor::onKey(Key key, ButtonState state)
{
}

void GameEditor::onUndo()
{ 	
}

void GameEditor::onRedo()
{

}

void GameEditor::onSave()
{
}

void GameEditor::setGame( const std::string & headerName )
{
	m_header = Assets::Load<Game::Header>(headerName );
	//if does not exist, create ? 
}

void GameEditor::setName( const std::string &name )
{
	if ( m_header )
	{
		m_header->name =  name;
	}

}
void GameEditor::package()
{
	if ( m_header )
		{
			Assets::Save<Game::Header>( m_header );
			const std::string path = FS::DirName( m_header->filepath );
			const std::string &cartPath = path + "/" + m_header->name + DEFAULT_CARTRIDGE_EXT;
			Game::Package( *m_header, cartPath );
		}
}

void GameEditor::requestRedrawHeader()
{
	m_requiresRedraw = true;

}

void GameEditor::AddComboBox( const std::string &labelText, std::vector<std::string> &names, int x, int y )
{
	Graphics::TextBox *label = new Graphics::TextBox( labelText.size(), 1, labelText, DEFAULT_FONT );
	label->x = x;
	label->y = y;
	label->textColor = WHITE; 
	label->fillColor = EDITOR_COLOR; 
	label->reload();
	m_labels.push_back( label );
	
	UI::ComboBox * comboBox = new UI::ComboBox( this, x+45, y, 16, 1 );
	for ( int i = 0; i < m_header->maps.size(); i++ )
	{
		comboBox->add( m_header->maps[i]);
	}
	
	comboBox ->cbSelect = [comboBox, &names] ( const std::string & text) { 
		names = comboBox->entries();
	};
	App::addWidget( comboBox  );
}

void GameEditor::redrawHeaderData()
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
	int offy = charH + 2;
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


