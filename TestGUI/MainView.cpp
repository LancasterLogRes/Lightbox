#include <Common/Common.h>
#include <GUI/TextLabel.h>
#include <GUI/ToggleButton.h>
#include <GUI/Slate.h>
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
		BasicButton b = BasicButtonBody::spawn(this, "Hello", Red);
		b->setGeometry(fRect(100, 60, 600, 360));
		b->setOnTapped([=](ViewBody* _v){ _v->update(); });
	}

private:
	virtual void initGraphics()
	{
//		setLayers({{ Layer(iMargin(), true) }});
	}

	virtual void draw(Slate const& _c, unsigned)
	{
		Color c(mod1(toSeconds(GUIApp::runningTime()) / 10), 1.f, 1.f);
		{
			fCoord p(GUIApp::style().thumbDiameter / 2.f);
			p += fSize(200, 0);
			_c.mmGlowThumb(p, Color(mod1(toSeconds(GUIApp::runningTime()) / 10), 1.f, 1.f));
			_c.mmGlowThumb(p + GUIApp::style().thumbDiameter * fSize(3, 0), c, 1.f);
		}
		_c.glowRectOutline(iRect(0, 100, 100, 100), c, 1.f);
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
