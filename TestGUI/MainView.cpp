#include <Common/Common.h>
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

class StringListModel: public ListModel
{
public:
	StringListModel(std::vector<std::string> const* _s): m_d(_s) {}

	virtual unsigned itemCount() { return m_d ? m_d->size() : 0; }
	virtual void drawItem(unsigned _i, Slate const& _s);
	virtual fSize itemSize(unsigned _i);

private:
	std::vector<std::string> const* m_d;
};

void StringListModel::drawItem(unsigned _i, Slate const& _s)
{
	if (m_d && _i < m_d->size())
	{
		_s.text(m_d->at(_i), AtLeft, _s.main().lerp(0, 0.5), White);
	}
}

fSize StringListModel::itemSize(unsigned _i)
{
	if (m_d && _i < m_d->size())
		return GUIApp::style().regular.measure(m_d->at(_i)).size();
	return fSize();
}

class TestViewBody: public ViewCreator<ViewBody, TestViewBody>
{
	typedef ViewCreator<ViewBody, TestViewBody> Super;

public:
	TestViewBody()
	{
		for (int i = 0; i < 99; ++i)
			m_l += "Item " + toString(i + 1);
		ListView l = ListViewBody::spawn(this, new StringListModel(&m_l));
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

	std::vector<std::string> m_l;
};

MainView::MainView()
{
	TestViewBody::spawn(this);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
