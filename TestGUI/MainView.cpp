#include <Common/Global.h>
#include <GUI/TextLabel.h>
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
		m_f = Font(100, "Ubuntu");
	}

private:
	virtual void draw(Context const& _c, unsigned)
	{
		fRect r = m_f.measure("Hello world", true).translated(fSize(100, 150));
		fRect r2 = m_f.measure("Hello world", false).translated(fSize(100, 250));
		fRect r3 = m_f.getBaked()->measureMm("Hello world").translated(fSize(100, 350));
		_c.rect(r, Color(0, 1, 1, .5f));
		_c.rect(r2, Color(.33, 1, 1, .5f));
		_c.rect(r3, Color(.66, 1, 1, .5f));
		m_f.draw(_c.toDevice(r.lerp(.5, .5)), "Hello world", RGBA::Red, AtCenter);
		m_f.draw(_c.toDevice(r2.lerp(.5, .5)), "Hello world", RGBA::Green, AtCenter);
		m_f.draw(_c.toDevice(r3.lerp(.5, .5)), "Hello world", RGBA::Blue, AtCenter);
	}

	virtual void initGraphics()
	{
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
