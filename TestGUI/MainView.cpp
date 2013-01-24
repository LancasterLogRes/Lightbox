#include <Common/Global.h>
#include <GUI/TextLabel.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "MainView.h"
using namespace std;
using namespace Lightbox;

MainView::MainView()
{
	TextLabelBody::spawn(this, "Hello, world!", GUIApp::style().high);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
