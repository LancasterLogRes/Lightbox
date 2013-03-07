#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Slate.h"
#include "Tabs.h"
using namespace std;
using namespace Lightbox;

static const float c_tabBodyHeight = 35;
static const float c_tabTailHeight = 5;
static const float c_tabHeight = c_tabBodyHeight + c_tabTailHeight;
static const float c_tabSelRectWidth = 2;

TabsBody::TabsBody()
{
	setLayout(new OverlayLayout(fMargin(0, c_tabHeight, 0, 0)));
}

TabsBody::~TabsBody()
{
}

void TabsBody::addTab(std::string const& _title, View const& _page)
{
	_page->show(!children().size());
	_page->setProperty("_TabsBody:Title", _title);
	_page->setParent(this);
}

bool TabsBody::event(Event* _e)
{
	if (TouchEvent* e = dynamic_cast<TouchDownEvent*>(_e))
	{
		fCoord pos = e->mmLocal() - geometry().pos();
		if (pos.y() > 0 && pos.y() < c_tabHeight && children().size())
		{
			fSize tabSize(geometry().width() / children().size(), c_tabHeight);
			fCoord cursor(0, 0);
			for (auto const& c: children())
			{
				c->show(fRect(cursor, tabSize).contains(pos));
				cursor.setX(cursor.x() + tabSize.w());
			}
			update();
			return true;
		}
	}
	return Super::event(_e);
}

void TabsBody::draw(Slate const& _c, unsigned)
{
	if (children().size())
	{
		fRect tab(fCoord(0, 0), fSize(geometry().width() / children().size(), c_tabHeight));
		iRect activeTabPx;
		iSize tailSizePx = GUIApp::joint().display->toUnalignedPixels(fSize(c_tabTailHeight));
		iSize selRectPx = GUIApp::joint().display->toUnalignedPixels(fSize(c_tabSelRectWidth));
		for (auto const& c: children())
		{
			iRect tabPx = GUIApp::joint().display->toPixels(tab);
			string title = c->property<string>("_TabsBody:Title");
			bool isActive = c->isShown();
			if (isActive)
			{
				activeTabPx = tabPx;
				iMargin m(tailSizePx.width(), tailSizePx.height(), tailSizePx.width(), 0);
				_c.rectInline(tabPx, m, White);
				iMargin sm(selRectPx.width(), selRectPx.height(), selRectPx.width(), 0);
				iRect usefulTab = tabPx.inset(m);
//				_c.rect(usefulTab.inset(sm), White);
				_c.text(GUIApp::style().bigBold, usefulTab.lerp(.5f, .5f), title, White);
			}
			else
			{
				iRect usefulTabPx = tabPx.inset(0, 0, 0, tailSizePx.height());
				_c.rect(usefulTabPx, Color(.1f), -.1f);
				_c.rect(usefulTabPx.lerp(0, .35f, 1, 1), Color(0.f, .2f));
				_c.text(GUIApp::style().big, usefulTabPx.lerp(.5f, .5f), title, Color(.5f));
			}
			tab.translate(fSize(tab.w(), 0));
		}
		_c.rect(iRect(iCoord(0, activeTabPx.bottom() - tailSizePx.height()), activeTabPx.bottomLeft()), White);
		_c.rect(iRect(activeTabPx.bottomRight() - iSize(0, tailSizePx.height()), iCoord(rect().right(), activeTabPx.bottom())), White);
	}
}


