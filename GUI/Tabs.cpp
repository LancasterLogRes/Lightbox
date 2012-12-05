#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Tabs.h"
using namespace std;
using namespace Lightbox;

TabsBody::TabsBody()
{
	setLayout(new OverlayLayout(fVector4(0, 20, 0, 0)));
}

TabsBody::~TabsBody()
{
}

void TabsBody::addTab(std::string const& _title, View const& _page)
{
	_page->setVisible(!m_children.size());
	_page->setProperty("_TabsBody:Title", _title);
	_page->setParent(this);
}

bool TabsBody::event(Event* _e)
{
	if (TouchEvent* e = dynamic_cast<TouchDownEvent*>(_e))
	{
		fCoord pos = e->local - geometry().pos();
		if (pos.y() > 0 && pos.y() < 20 && m_children.size())
		{
			fSize tabSize(geometry().width() / m_children.size(), 20);
			fCoord cursor(0, 0);
			for (auto const& c: m_children)
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
	if (m_children.size())
	{
		fSize tabSize(geometry().width() / m_children.size(), 20);
		fCoord cursor = _c.offset + geometry().pos();
		for (auto const& c: m_children)
		{
			string title = c->property<string>("_TabsBody:Title");
			bool isActive = c->isVisible();
			(isActive ? GUIApp::style().bold : GUIApp::style().regular).draw(fRect(cursor, tabSize).lerp(.5, .5), title, RGBA(GUIApp::style().fore));
			cursor.setX(cursor.x() + tabSize.w());
		}
	}
}


