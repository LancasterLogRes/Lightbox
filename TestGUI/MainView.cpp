#include <Common/Common.h>
#include <GUI/TextLabel.h>
#include <GUI/ToggleButton.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "MainView.h"
using namespace std;
using namespace Lightbox;

class TestViewBody;
typedef boost::intrusive_ptr<TestViewBody> TestView;

class TestViewBody: public ViewCreator<ViewBody, TestViewBody>
{
	typedef ViewCreator<ViewBody, TestViewBody> Super;

public:
	TestViewBody()
	{
//		m_f = Font(100, "ubuntu");
		ToggleButtonBody::spawn(this, "Hello", Red)->setGeometry(fRect(400, 20, 150, 50));
	}

private:
	virtual void initGraphics()
	{
	}

	virtual void draw(Context const& _c, unsigned)
	{
		fCoord p(GUIApp::style().thumbDiameter / 2.f);
		_c.blitThumb(p, Color(mod1(toSeconds(GUIApp::runningTime()) / 10), 1.f, 1.f));
		_c.blitThumb(p + GUIApp::style().thumbDiameter * fSize(3, 0), Color(mod1(toSeconds(GUIApp::runningTime()) / 10), 1.f, 1.f), 1.f);
	}

	virtual void finiGraphics()
	{
	}

	Font m_f;
};

MainView::MainView()
{
	TestViewBody::spawn(this);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
