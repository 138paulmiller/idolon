
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


	Editor::addTool("PACKAGE", [&](){
		if ( m_header )
		{
			Assets::Save<Game::Header>( m_header );
			const std::string path = FS::DirName( m_header->filepath );
			const std::string &cartPath = path + "/" + m_header->name + DEFAULT_CARTRIDGE_EXT;
			Game::Package( *m_header, cartPath );
		}
	}, false);
	

}

void GameEditor::onExit()
{
	m_headerButtons.clear();
	App::clear();
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
}

//
void GameEditor::onKey(Key key, ButtonState state)
{
}

void GameEditor::undo()
{ 	
}

void GameEditor::redo()
{

}

void GameEditor::save()
{
}

void GameEditor::setGame( const std::string & headerName )
{
	m_header = Assets::Load<Game::Header>(headerName );
}

void GameEditor::setName( const std::string &name )
{
	if ( m_header )
	{
		m_header->name =  name;
	}

}

void GameEditor::openTilesetInputs()
{
	//display all tilesetnames, plus a text input with ... to add a new one

	//on click, set tileset
	//if any of these are clicked, or leave then close
}

void GameEditor::closeTilesetInputs()
{}	

void GameEditor::requestRedrawHeader()
{
	m_requiresRedraw = true;

}

void GameEditor::redrawHeaderData()
{
	m_requiresRedraw = false;
	for ( int i = 0; i < m_headerButtons.size(); i++ )
	{
		App::removeButton( m_headerButtons[i] );
	}
	m_headerButtons.clear();

	//TODO add + buttons to add
	// when adding will redraw

	//use just to get w/h
	const std::string &fontName = DEFAULT_FONT;
	Graphics::Font *font = Assets::Load<Graphics::Font>( DEFAULT_FONT );
	const int charW = font->charW;
	const int charH = font->charH;
	Assets::Unload<Graphics::Font>( fontName );

	const std::string &name = m_header ? m_header->name : "";

	int inputw = 15;
	int colx = charW;
	int offy = charH + 2;
	int y = 10;

	//name info
	{
		const std::string &label = "Game Name";
		m_headerButtons.push_back( App::addButton( new UI::TextButton( label, 0, y, label.size(), 1, DEFAULT_FONT ) ) );
		y += offy;

		UI::TextInput *nameInput = new UI::TextInput( name, colx, y, inputw, 1, DEFAULT_FONT );
		nameInput->cbAccept = [this, nameInput] () {
			this->setName( nameInput->text );
			//display all tilesetnames, plus a text input with ... to add a new one
		};
		m_headerButtons.push_back( App::addButton( nameInput ) );
		y += offy;
	}

	{//tileset info

		const std::string &label = "Tilesets";
		m_headerButtons.push_back( App::addButton( new UI::TextButton( label, 0, y, label.size(), 1, DEFAULT_FONT ) ));

		UI::TextButton * addTilesetButton = new UI::TextButton( "+", (2+label.size()) * charW, y, 1, 1, DEFAULT_FONT );
		addTilesetButton->cbClick = [this] () {
			if ( this->m_header ) this->m_header->tilesets.push_back( "" );
			this->requestRedrawHeader();
		};
		m_headerButtons.push_back(App::addButton( addTilesetButton ));
		y += offy;

		if ( m_header )
		{
			for ( int i = 0; i < m_header->tilesets.size(); i++ )
			{
				UI::TextInput * tilesetInput = new UI::TextInput( m_header->tilesets[i], colx, y, inputw, 1, DEFAULT_FONT);
				y += offy;
				tilesetInput->cbAccept = [this, i, tilesetInput] () {
					this->m_header->tilesets[i] = tilesetInput->text;
				};
				m_headerButtons.push_back(App::addButton( tilesetInput ));
			}
		}
	}

}
