#pragma once 

#include "ui.hpp"

//////////////////////////////////////////////////////////////////////////////////

namespace UI
{
	class Widget;
	class Button;
}

class App
{
public:
	App();
	virtual ~App();
	virtual void onEnter() = 0;
	virtual void onExit() = 0;
	virtual void onTick() = 0;
	virtual void onKey(Key key, ButtonState state) = 0;
						

	void signal(AppCode code);
	AppCode status();

	void update();
	void draw();

	void clear();

	int addWidget(UI::Widget * widget);
	int addButton(UI::Button * button);

	UI::Button * getButton(int idx);
	UI::Widget * getWidget(int idx);

	int getButtonCount();
	int getWidgetCount();

	void removeButton(int idx);
	void removeWidget(int idx);


private:
	bool m_isFocused;//is current focus

	AppCode m_status;
	std::vector<UI::Widget*> m_widgets;
	std::vector<UI::Button*> m_buttons;
};

//App context
class Context
{
public:
	Context( uint8 appCount );
	~Context();
	void clear();
	void create( uint8 appId, App * app );
	//kill previous process
	void enter(uint8 appId, bool kill );
	void exit();
	void handleKey( Key key, ButtonState state);
	AppCode run( );
	


	template <typename Type= App>
	Type * app()
	{
		return dynamic_cast<Type*>(m_app);
	}

	template <typename Type>
	Type * app(int appId)
	{
		return dynamic_cast<Type*>(m_apps[appId]);
	}

private:

	uint8 m_appId;
	std::vector<uint8> m_appStack;
	App * m_app;	

	uint8 m_appCount;
	App ** m_apps = { 0} ;
};