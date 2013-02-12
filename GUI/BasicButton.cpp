#include <array>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "GUIApp.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

BasicButtonBody::BasicButtonBody(std::string const& _text, Color _c, Font _f, Grouping _grouping):
	m_text(_text),
	m_color(_c),
	m_font(_f),
	m_grouping(_grouping),
	m_isLit(false),
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

Layers BasicButtonBody::layers()
{
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));
	return {{ Layer(), Layer(iMargin(lightWidth * 2, lightWidth * 2), false) }};
}

void BasicButtonBody::initGraphics()
{
	setLayers(layers());
	updateLayers();
}

void BasicButtonBody::setLit(bool _lit)
{
	if (m_isLit != _lit)
	{
		m_isLit = _lit;
		updateLayers();
	}
}

void BasicButtonBody::setDown(bool _down)
{
	if (m_isDown != _down)
	{
		m_isDown = _down;
		layer(0).update();
	}
}

void BasicButtonBody::drawButton(Context const& _c, unsigned _l, bool _down, function<void(iRect)> const& _inner, bool _polish)
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
			_c.text(m_font.isValid() ? m_font : _down ? GUIApp::style().bold : GUIApp::style().regular, inner.lerp(.5f, .5f), boost::algorithm::to_upper_copy(m_text), Color(m_color.hue(), m_color.sat() * .75f, m_color.value() * .75f).toRGBA());
		if (!_down && _polish)
		{
			_c.rect(inner.lerp(0, 0, 1, .35f), Color(1.f, .05f));
		}
	}
	else if (_l == 1)
	{
		if (_inner)
			_inner(inner);
		else
			_c.text(m_font.isValid() ? m_font : GUIApp::style().bold, inner.lerp(.5f, .5f), boost::algorithm::to_upper_copy(m_text), color().toRGBA());
		_c.glowRectOutline(inner, color());
	}
	else if (_inner)
		_inner(inner);
}

void BasicButtonBody::draw(Context const& _c, unsigned _l)
{
	drawButton(_c, _l, m_isDown);
}

bool BasicButtonBody::event(Event* _e)
{
	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
		cdebug << m_text << ":" << boolalpha << e->id << "DOWN at" << e->local << geometry().contains(e->local);
		m_downPointer = e->id;
		lockPointer(e->id);
		pushed();
		return true;
	}
	else if (auto e = dynamic_cast<TouchUpEvent*>(_e))
	{
		cdebug << m_text << ":" << boolalpha << e->id << "UP at" << e->local << m_isDown << (e->id == m_downPointer) << geometry().contains(e->local);
		if (m_isDown && e->id == m_downPointer)
		{
			m_downPointer = -1;
			released(geometry().contains(e->local));
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

void BasicButtonBody::enabledChanged()
{
	Super::enabledChanged();
	updateLayers();
}

fSize BasicButtonBody::specifyMinimumSize(fSize) const
{
	// Until Fonts work without renderer.
	return GUIApp::get() ? GUIApp::style().bold.measure(m_text).size() : fSize(0, 0);
}
