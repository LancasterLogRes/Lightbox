#pragma once

#include <App.h>
#include <GUI/GUIApp.h>
#include "MainView.h"
using namespace std;
using namespace Lightbox;

namespace Lightbox
{

class TestGUIApp: public GUIApp
{
	typedef GUIApp Super;

public:
	TestGUIApp();

	virtual void initGraphics(Display& _d);
	virtual void go();
	virtual void sendInitialEvents();

	static TestGUIApp* get() { return dynamic_cast<TestGUIApp*>(GUIApp::get()); }

	virtual void tick();

private:
	boost::intrusive_ptr<MainView> m_mainView;
};

}


