#include <Common/Common.h>
#include <Common/thread.h>
#include <GUI/TextLabel.h>
#include <GUI/ToggleButton.h>
#include <GUI/ListView.h>
#include <GUI/HuePicker.h>
#include <GUI/Slate.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "MainView.h"
using namespace std;
using namespace Lightbox;

class TestViewBody;
typedef boost::intrusive_ptr<TestViewBody> TestView;

/*
class QuickListModel: public ListModel
{
public:
	QuickListModel(std::function<unsigned()> _itemCount, std::function<void(unsigned, Slate const&)> _drawItem): m_itemCount(_itemCount), m_drawItem(_drawItem) {}

	virtual unsigned itemCount() { return m_itemCount(); }
	virtual void drawItem(unsigned _i, Slate const& _s) { m_drawItem(_i, _s); }
	virtual fSize itemSize(unsigned _i) { return m_itemSize(_i); }

private:
	std::function<unsigned()> m_itemCount;
	std::function<void(unsigned, Slate const&)> m_drawItem;
};
*/

class StringListAdaptor: public ListModel
{
public:
	StringListAdaptor(std::vector<std::string> const* _s): m_d(_s) {}

	virtual unsigned itemCount() { return m_d ? m_d->size() : 0; }
	virtual void drawItem(unsigned _i, Slate const& _s, bool _selected);
	virtual fSize itemSize(unsigned _i);

private:
	std::vector<std::string> const* m_d;
};

void StringListAdaptor::drawItem(unsigned _i, Slate const& _s, bool _selected)
{
	if (m_d && _i < m_d->size())
	{
		_s.text(m_d->at(_i), AtLeft, _s.main().inset(_s.toPixels(fSize(3, 3))).lerp(0, 0.5), White, GUIApp::style().small);
	}
}

fSize StringListAdaptor::itemSize(unsigned _i)
{
	if (m_d && _i < m_d->size())
		return GUIApp::style().small.measure(m_d->at(_i)).size() + fSize(6, 6);
	return fSize();
}

typedef std::pair<std::string, std::string> DoubleString;
typedef std::vector<DoubleString> DoubleStrings;
class DoubleStringsAdaptor: public ListModel
{
public:
	DoubleStringsAdaptor(DoubleStrings const* _s): m_d(_s) {}

	virtual unsigned itemCount() { return m_d ? m_d->size() : 0; }
	virtual void drawItem(unsigned _i, Slate const& _s, bool _selected);
	virtual fSize itemSize(unsigned _i);

private:
	DoubleStrings const* m_d;
};

void DoubleStringsAdaptor::drawItem(unsigned _i, Slate const& _s, bool _selected)
{
	if (m_d && _i < m_d->size())
	{
		_s.text(m_d->at(_i).first, AtLeft|AtTop, _s.main().inset(_s.toPixels(fSize(3, 3))).lerp(0, 0), Color(_selected ? 1.f : .5f), GUIApp::style().small);
		_s.text(m_d->at(_i).second, AtLeft|AtBottom, _s.main().inset(_s.toPixels(fSize(3, 3))).lerp(0, 1), Color(_selected ? .75f : .25f), GUIApp::style().xsmall);
	}
}

fSize DoubleStringsAdaptor::itemSize(unsigned _i)
{
	if (m_d && _i < m_d->size())
		return GUIApp::style().small.measure(m_d->at(_i).first).size().vStacked(GUIApp::style().xsmall.measure(m_d->at(_i).second).size()) + fSize(6, 6);
	return fSize();
}

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
