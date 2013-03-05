#include <Common/Common.h>
#include <Common/thread.h>
#include <GUI.h>
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
		for (int i = 0; i < 99; ++i)
			m_l += DoubleString("Item " + toString(i + 1), "Subtext for item " + toString(i + 1));
		ListView l = ListViewBody::spawn(this, new DoubleStringsAdaptor(&m_l));
		l->setGeometry(fRect(0, 0, 200, 480));
		MarkdownView m = MarkdownViewBody::spawn(this, "Hello world! This is a nice long piece of text for the MarkdownView object to wrap nicely. It has multiple sentences and even...\nA new-line!");
		m->setGeometry(fRect(200, 50, 600, 430));
		DropListView d = DropListViewBody::spawn(this, new DoubleStringsAdaptor(&m_l));
		d->setGeometry(fRect(200, 0, 600, 50));
	}

private:
	virtual void initGraphics()
	{
	}

	virtual void draw(Slate const&, unsigned)
	{
	}

	virtual void finiGraphics()
	{
	}

	DoubleStrings m_l;
};

MainView::MainView()
{
	TestViewBody::spawn(this);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
