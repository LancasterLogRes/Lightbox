#include <Common/Global.h>
#include <Common/thread.h>
#include <GUI/TextLabel.h>
#include <GUI/Font.h>
#include "Global.h"
#include "TestGUIApp.h"
using namespace std;
using namespace Lightbox;

TestGUIApp::TestGUIApp()
{
	Lightbox::g_debugEnabled[SnoopGL] = 0;	// Enable/disable GL snoop.
}

void TestGUIApp::initGraphics(Display& _d)
{
	GUIApp::initGraphics(_d);
}

void TestGUIApp::sendInitialEvents()
{
}

void TestGUIApp::go()
{
	Super::go();

	m_mainView = new MainView;
	m_mainView->setParent(root());
	root()->setLayout(new OverlayLayout);
}

void TestGUIApp::tick()
{
}
