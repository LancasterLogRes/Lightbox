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
		return b->grouping() & HorizontalGrouping || b->grouping() & VerticalGrouping;
	return false;
}

static const float c_surroundWidth = 2;
static const float c_lightWidth = 4;

vector<iMargin> BasicButtonBody::prepareDraw()
{
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));
	vector<iMargin> ret;
	ret += iMargin();
	if (isEnabled())
		ret += iMargin(lightWidth / 2, lightWidth - lightWidth / 2);
	return ret;
}

void BasicButtonBody::drawButton(Context const& _c, unsigned _l, bool _lit, bool _down, function<void(iRect)> const& _inner, bool _polish)
{
	iSize surroundWidth = GUIApp::joint().display->toPixels(fSize(c_surroundWidth, c_surroundWidth));
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));

	bool haveLeft = m_grouping & ForceLeft || (m_grouping & HorizontalGrouping && childIndex() > 0 && isNeighbour(parent()->child(childIndex() - 1)));
	bool haveRight = m_grouping & ForceRight || (m_grouping & HorizontalGrouping && isNeighbour(parent()->child(childIndex() + 1)));
	bool haveTop = m_grouping & ForceAbove || (m_grouping & VerticalGrouping && childIndex() > 0 && isNeighbour(parent()->child(childIndex() - 1)));
	bool haveBottom = m_grouping & ForceBelow || (m_grouping & VerticalGrouping && isNeighbour(parent()->child(childIndex() + 1)));

	iRect surround = rect();
	iMargin surroundMargin(haveLeft ? 0 : surroundWidth.w(), haveTop ? 0 : surroundWidth.h(), haveRight ? 0 : surroundWidth.w(), haveBottom ? 0 : surroundWidth.h());
	iMargin lightMargin(lightWidth.w() / (haveLeft ? 2 : 1), lightWidth.h() / (haveTop ? 2 : 1), lightWidth.w() - (haveRight ? lightWidth.w() / 2 : 0), lightWidth.h() - (haveBottom ? lightWidth.h() / 2 : 0));
	iRect outer = surround.inset(surroundMargin);
	iRect inner = outer.inset(lightMargin);
	if (_l == 0)
	{
		_c.rectOutline(outer, surroundMargin, Color(0));
		_c.rectOutline(inner, lightMargin, Color(m_color.hue(), m_color.sat(), .125f));
		_c.rect(inner, Color(_down ? .05f : .1f), _down ? .05f : -.1f);
		if (_inner)
			_inner(inner);
		else
			_c.text(GUIApp::style().bold, inner.lerp(.5f, .5f), boost::algorithm::to_upper_copy(m_text), Color(m_color.hue(), m_color.sat() * .75f, 1.f).toRGBA());
		if (!_down && _polish)
		{
			_c.rect(inner.lerp(0, 0, 1, .35f), Color(1.f, .05f));
		}
	}
	else if (_l == 1 && _lit)
	{
		_c.rectOutline(inner.outset(lightWidth / 4), iMargin(lightWidth / 2), m_color);
	}
}

void BasicButtonBody::draw(Context const& _c, unsigned _l)
{
	drawButton(_c, _l, m_isDown, m_isDown);
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
