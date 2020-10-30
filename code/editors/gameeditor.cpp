
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
	
	Editor::addTool("CODE", [this](){
		const std::string &name = m_header->scripts.empty() ? "" : m_header->scripts[0];
		Sys::RunScriptEditor( name);
	}, true);
	
	Editor::addTool("TILESET", [this](){
		const std::string &name = m_header->tilesets.empty() ? "" : m_header->tilesets[0];
		Sys::RunTilesetEditor( name);
	}, true);
	
	Editor::addTool("MAP", [this](){
		const std::string &name = m_header->maps.empty() ? "" : m_header->maps[0];
		Sys::RunMapEditor( name );
	}, true);
	
	Editor::addTool("BUILD", [this](){
		this->package();
	}, false);
}

void GameEditor::onExit()
{
	m_headerButtons.clear();
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


int GameEditor::AddNamesList(const std::string & labelText, std::vector<std::string> &names, int x, int y, int colx, int offy, int inputw )
{
	Graphics::TextBox *label = new Graphics::TextBox( labelText.size(), 1, labelText, DEFAULT_FONT );
	label->x = x;
	label->y = y;
	label->textColor = WHITE; 
	label->fillColor = EDITOR_COLOR; 
	label->reload();
 
	m_labels.push_back( label );

	UI::TextButton * addTilesetButton = new UI::TextButton( "+", colx, y, 1, 1, DEFAULT_FONT );
	addTilesetButton->cbClick = [this, &names] () {

		if ( names.size() < MAX_TILESET_COUNT ) {
			names.push_back( "" );
			this->requestRedrawHeader();
		}
	};
	m_headerButtons.push_back(App::addButton( addTilesetButton ));

	UI::TextButton * removeTilesetButton = new UI::TextButton( "-", colx + 10, y, 1, 1, DEFAULT_FONT );
	removeTilesetButton->cbClick = [this, &names] () {

		if ( names.size() > 0) {
			names.pop_back( );
			this->requestRedrawHeader();
		}
	};
	m_headerButtons.push_back(App::addButton( removeTilesetButton ));


	y += offy;

	if ( m_header )
	{
		for ( int i = 0; i < names.size(); i++ )
		{
			UI::TextInput * textInput = new UI::TextInput( names[i], colx, y, inputw, 1, DEFAULT_FONT);
			y += offy;
			textInput->cbAccept = [this, i, textInput, &names] () {
				names[i] = textInput->text;
			};
			m_headerButtons.push_back(App::addButton( textInput ));

			//add a goto button, this will load editor on the tileset, if the tileset does not exist, it will be created
		}
	}
	return y + offy;
	
}

void GameEditor::redrawHeaderData()
{
	if ( m_header == nullptr ) return;

	m_requiresRedraw = false;
	for ( int i = 0; i < m_headerButtons.size(); i++ )
	{
		App::removeButton( m_headerButtons[i] );
	}
	m_headerButtons.clear();

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
	int borderx = charW * 3;
	int colx = charW*15;
	int offy = charH + 2;
	int y = 20;

	//name info
	{
		const std::string &labelText = "Game Name";
		Graphics::TextBox *label = new Graphics::TextBox( labelText.size(), 1, labelText, DEFAULT_FONT );
		label->x = borderx;
		label->y = y;
		label->textColor = WHITE; 
		label->fillColor = EDITOR_COLOR; 
		label->reload();

		m_labels.push_back( label );
		
		UI::TextInput *nameInput = new UI::TextInput( name, colx, y, inputw, 1, DEFAULT_FONT );
		nameInput->cbAccept = [this, nameInput] () {
			this->setName( nameInput->text );
			//display all tilesetnames, plus a text input with ... to add a new one
		};
		m_headerButtons.push_back( App::addButton( nameInput ) );
		y += offy;
	}
	y += offy;

	y = AddNamesList( "Maps", m_header->maps, borderx, y, colx, offy, inputw );
	y = AddNamesList( "Scripts", m_header->scripts, borderx, y, colx, offy, inputw );
	y = AddNamesList( "Fonts", m_header->fonts, borderx, y, colx, offy, inputw );

}


