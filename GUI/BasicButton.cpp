#include <array>
#include <unordered_map>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "GUIApp.h"
#include "RenderToTextureSlate.h"
#include "BasicButton.h"
using namespace std;
using namespace lb;

BasicButtonBody::BasicButtonBody(Color _c, Grouping _grouping):
	m_color(_c),
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
/*
class TextureCache
{
public:
	TextureCache() {}
	~TextureCache() {}

	void initGraphics();
	void finiGraphics();

	std::unordered_map<std::hash_t, Texture2D> m_cached;
};
static TextureCache s_tc;
*/

Layers BasicButtonBody::layers()
{
	return {{ Layer(), Layer(borderMargin(), Premultiplied) }};
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

iMargin lb::innerMargin(Grouping _grouping)
{
	auto lightBedPixels = GUIApp::joint().lightBedPixels;
	return iMargin(lightBedPixels.w() / ((_grouping & ForceLeft) ? 2 : 1), lightBedPixels.h() / ((_grouping & ForceAbove) ? 2 : 1), lightBedPixels.w() - ((_grouping & ForceRight) ? lightBedPixels.w() / 2 : 0), lightBedPixels.h() - ((_grouping & ForceBelow) ? lightBedPixels.h() / 2 : 0));
}

iMargin lb::borderMargin()
{
	return iMargin(GUIApp::joint().glowPixels + GUIApp::joint().lightBedPixels / 2 + GUIApp::joint().lightEdgePixels);
}

void lb::drawBorder(Slate const& _con, iRect _inner, bool _base, bool _lit, Color _col)
{
	auto lightBedPixels = GUIApp::joint().lightBedPixels;
	auto lightEdgePixels = GUIApp::joint().lightEdgePixels;
	if (_base)
		_con.rectOutline(_inner, iMargin(lightBedPixels), _col.attenuated(.125f));
	if (_lit)
		_con.glowRectOutline(_inner.outset(iMargin((lightBedPixels - lightEdgePixels + iSize(1, 1)) / 2, (lightBedPixels - lightEdgePixels) / 2)), _col);
}

void lb::drawButton(Slate const& _c, iRect _inner, Color _color, bool _down, bool _base, bool _lit, bool _polish, bool _border)
{
	if (_base)
	{
		// Border fill
		if (_border)
			drawBorder(_c, _inner, true, false, _color);

		// Button texture
		_c.rect(_inner, Color(_down ? .05f : .1f), _down ? .05f : -.1f);
		if (!_down && _polish)
			_c.rect(_inner.lerp(0, 0, 1, .35f), Color(1.f, .05f));
	}

	if (_lit)
		// Border light
		drawBorder(_c, _inner, false, true, _color);
}

Grouping BasicButtonBody::effectiveGrouping() const
{
	Grouping ret = grouping();
	if (ret & HorizontalGrouping && childIndex() > 0 && isNeighbour(parent()->child(childIndex() - 1)))
		ret |= ForceLeft;
	if (ret & HorizontalGrouping && isNeighbour(parent()->child(childIndex() + 1)))
		ret |= ForceRight;
	if (ret & VerticalGrouping && childIndex() > 0 && isNeighbour(parent()->child(childIndex() - 1)))
		ret |= ForceAbove;
	if (ret & VerticalGrouping && isNeighbour(parent()->child(childIndex() + 1)))
		ret |= ForceBelow;
	return ret;
}

void BasicButtonBody::draw(Slate const& _c, unsigned _l)
{
	auto inner = rect().inset(innerMargin(effectiveGrouping()));
	drawButton(_c, inner, m_color, m_isDown, _l == 0, _l == 1, true);
}

bool BasicButtonBody::event(Event* _e)
{
	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
		cdebug << "Button:" << boolalpha << e->id << "DOWN at" << e->mmLocal() << geometry().contains(e->mmLocal());
		m_downPointer = e->id;
		lockPointer(e->id);
		pushed();
		return true;
	}
	else if (auto e = dynamic_cast<TouchUpEvent*>(_e))
	{
		cdebug << "Button:" << boolalpha << e->id << "UP at" << e->mmLocal() << m_isDown << (e->id == m_downPointer) << geometry().contains(e->mmLocal());
		if (m_isDown && e->id == m_downPointer)
		{
			m_downPointer = -1;
			released(geometry().contains(e->mmLocal()));
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
