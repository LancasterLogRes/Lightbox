#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Tabs.h"
using namespace std;
using namespace Lightbox;

static const float c_tabHeight = 40;

TabsBody::TabsBody()
{
	setLayout(new OverlayLayout(fVector4(0, c_tabHeight, 0, 0)));
}

TabsBody::~TabsBody()
{
}

void TabsBody::addTab(std::string const& _title, View const& _page)
{
	_page->setVisible(!children().size());
	_page->setProperty("_TabsBody:Title", _title);
	_page->setParent(this);
}

bool TabsBody::event(Event* _e)
{
	if (TouchEvent* e = dynamic_cast<TouchDownEvent*>(_e))
	{
		fCoord pos = e->local - geometry().pos();
		if (pos.y() > 0 && pos.y() < c_tabHeight && children().size())
		{
			fSize tabSize(geometry().width() / children().size(), c_tabHeight);
			fCoord cursor(0, 0);
			for (auto const& c: children())
			{
				c->setVisible(fRect(cursor, tabSize).contains(pos));
				cursor.setX(cursor.x() + tabSize.w());
			}
			update();
			return true;
		}
	}
	return Super::event(_e);
}

void TabsBody::draw(Context const& _c)
{
	Super::draw(_c);
	if (children().size())
	{
		fRect tab(_c.offset + geometry().pos(), fSize(geometry().width() / children().size(), c_tabHeight));
		for (auto const& c: children())
		{
			string title = c->property<string>("_TabsBody:Title");
			bool isActive = c->isVisible();
			_c.shaded(tab, isActive ? GUIApp::style().high : GUIApp::style().back);
			Font const& f = (isActive ? GUIApp::style().bold : GUIApp::style().regular);
			f.draw(tab.lerp(.5f, .5f) + fSize(0, -1), title, RGBA(0.f, 0.f, 0.f, .9f));
			f.draw(tab.lerp(.5f, .5f), title, RGBA(GUIApp::style().fore * 2.f));
			tab.translate(fSize(tab.w(), 0));
		}
	}
}


