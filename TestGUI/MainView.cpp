#include <Common/Common.h>
#include <Common/thread.h>
#include <GUI.h>
#include "Global.h"
#include "MainView.h"
using namespace std;
using namespace lb;

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
		MarkdownView m = MarkdownViewBody::spawn(this, "Hello world! This is a \\^\\bnice long piece of text\\. for the MarkdownView object to \\^\\'f#aaa'wrap\\. nicely. It has multiple sentences and even...\\nA new-line!");
		m->setGeometry(fRect(200, 50, 600, 100));
		TextLabel t = TextLabelBody::spawn(this, "Hello world! This is a nice long piece of text for the MarkdownView object to wrap nicely. It\nhas multiple sentences and even...\nA new-line!", NullColor, Font(), AlignTop|AlignLeft);
		t->setGeometry(fRect(200, 150, 600, 100));
		DropListView d = DropListViewBody::spawn(this, new DoubleStringsAdaptor(&m_l));
		d->setGeometry(fRect(200, 0, 600, 50));
	}

private:
	virtual void initGraphics()
	{
		Font f = GUIApp::style().big;
		cdebug << f.getBaked()->pxSize() << f.getBaked()->pxMeasure("x");
		cdebug << GUIApp::fontManager().getInfo(f.definition()).metrics('x', ' ', f.getBaked()->pxSize());
		cdebug << f.pxSize() << f.pxMetrics('x', ' ', false);
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
