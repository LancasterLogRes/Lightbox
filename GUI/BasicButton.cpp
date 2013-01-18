#include <array>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "GUIApp.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

BasicButtonBody::BasicButtonBody(std::string const& _text, Color _c, Grouping _grouping):
	m_text(_text),
	m_color(_c),
	m_grouping(_grouping),
	m_isDown(false)
{
}

bool isNeighbour(View const& _v)
{
	if (BasicButtonBody* b = dynamic_cast<BasicButtonBody*>(_v.get()))
		return b->grouping() != BasicButtonBody::NoGrouping;
	return false;
}

static const float c_surroundWidth = 2;
static const float c_lightWidth = 4;

iMargin BasicButtonBody::prepareDraw(int)
{
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));
	return iMargin(lightWidth / 2, lightWidth - lightWidth / 2);
}

void BasicButtonBody::draw(Context const& _c, int)
{
	iSize surroundWidth = GUIApp::joint().display->toPixels(fSize(c_surroundWidth, c_surroundWidth));
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));

	bool haveLeft = (m_grouping == Horizontal && childIndex() > 0 && isNeighbour(parent()->child(childIndex() - 1)));
	bool haveRight = (m_grouping == Horizontal && isNeighbour(parent()->child(childIndex() + 1)));
	bool haveTop = (m_grouping == Vertical && childIndex() > 0 && isNeighbour(parent()->child(childIndex() - 1)));
	bool haveBottom = (m_grouping == Vertical && isNeighbour(parent()->child(childIndex() + 1)));

	iRect surround = _c.pixels(this);
	iRect outer = surround.inset(haveLeft ? 0 : surroundWidth.w(), haveTop ? 0 : surroundWidth.h(), haveRight ? 0 : surroundWidth.w(), haveBottom ? 0 : surroundWidth.h());
	iRect inner = outer.inset(lightWidth.w() / (haveLeft ? 2 : 1), lightWidth.h() / (haveTop ? 2 : 1), lightWidth.w() - (haveRight ? lightWidth.w() / 2 : 0), lightWidth.h() - (haveBottom ? lightWidth.h() / 2 : 0));
	_c.rect(surround, Color(0));
	_c.rect(outer, m_isDown ? m_color : Color(m_color.hue(), .5f, .125f));
	if (!m_isDown)
	{
		_c.rect(inner.lerp(0, 0, 1, .35f), Color(.125f), -.2f);
		_c.rect(inner.lerp(0, .35f, 1, 1), Color(0.0625f), -.1f);
	}
	else
		_c.rect(inner, Color(0.0625f), -.1f);
	_c.text(GUIApp::style().bold, inner.lerp(.5f, .5f), m_text, (m_isDown ? m_color : Color(m_color.hue(), .125f, .625f)).toRGBA());
}

bool BasicButtonBody::event(Event* _e)
{
	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
		cdebug << m_text << ":" << boolalpha << e->id << "DOWN at" << e->local << geometry().contains(e->local);
		m_isDown = true;
		m_downPointer = e->id;
		lockPointer(e->id);
		update();
		return true;
	}
	else if (auto e = dynamic_cast<TouchUpEvent*>(_e))
	{
		cdebug << m_text << ":" << boolalpha << e->id << "UP at" << e->local << m_isDown << (e->id == m_downPointer) << geometry().contains(e->local);
		if (m_isDown && e->id == m_downPointer)
		{
			if (geometry().contains(e->local))
				tapped();
			m_downPointer = -1;
			m_isDown = false;
			update();
		}
		return true;
	}
	return false;
}

void BasicButtonBody::tapped()
{
	if (m_onTapped)
		m_onTapped(this);
}

fSize BasicButtonBody::specifyMinimumSize(fSize) const
{
	// Until Fonts work without renderer.
	return GUIApp::get() ? GUIApp::style().bold.measure(m_text) : fSize(0, 0);
}
