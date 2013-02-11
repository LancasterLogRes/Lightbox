#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "DirectionPicker.h"
using namespace std;
using namespace Lightbox;

DirectionPickerBody::DirectionPickerBody(Color _c, Grouping _grouping):
	m_direction(0.5f, 0.5f, 0.5f),
	m_mode(Point),
	m_color(_c),
	m_grouping(_grouping),
	m_lastSign(-1, -1)
{
}

DirectionPickerBody::~DirectionPickerBody()
{
}

bool DirectionPickerBody::event(Event* _e)
{
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (e)
	{
		fCoord p = fCoord(fSize(e->local - geometry().pos()) / fSize(geometry().size())).clamped(0.f, 1.f);
		if (dynamic_cast<TouchDownEvent*>(e))
		{
			lockPointer(e->id);
			float dC = (m_direction.pos() - p).length() * .5f; // slight bonus, since the are inevitably easier to select.
			m_dragCenter = (m_mode == Point || ((dC < sqr(xC(1) - p.x()) + sqr(yC(1) - p.y())) && (dC < sqr(xC(-1) - p.x()) + sqr(yC(1) - p.y())) &&
					(dC < sqr(xC(-1) - p.x()) + sqr(yC(-1) - p.y())) && (dC < sqr(xC(1) - p.x()) + sqr(yC(-1) - p.y()))));
		}
		if (pointerLocked(e->id))
		{
			if (m_dragCenter)
				setPos(p, true);
			else
			{
				m_lastSign = iSize((p - m_direction.pos()).sign());
				fSize r(min(.5f, wC(p.x())), min(.5f, hC(p.y())));
				if (fabs(r.w()) < .05f)
					r.setW(0.f);
				if (fabs(r.h()) < .05f)
					r.setH(0.f);
				if (r == fSize(0.f, 0.f))
					r.setW(.05f);
				setRadii(r, true);
			}
			update();
			return true;
		}
	}
	return Super::event(_e);
}

//static const float c_surroundWidth = 2;
//static const float c_lightWidth = 4;

void DirectionPickerBody::initGraphics()
{
	Layers l = BasicButtonBody::layers();
	iSize surroundWidth = GUIApp::joint().display->toPixels(fSize(c_surroundWidth, c_surroundWidth));
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));
	bool haveLeft = m_grouping & ForceLeft;
	bool haveRight = m_grouping & ForceRight;
	bool haveTop = m_grouping & ForceAbove;
	bool haveBottom = m_grouping & ForceBelow;
	iMargin surroundMargin(haveLeft ? 0 : surroundWidth.w(), haveTop ? 0 : surroundWidth.h(), haveRight ? 0 : surroundWidth.w(), haveBottom ? 0 : surroundWidth.h());
	iMargin lightMargin(lightWidth.w() / (haveLeft ? 2 : 1), lightWidth.h() / (haveTop ? 2 : 1), lightWidth.w() - (haveRight ? lightWidth.w() / 2 : 0), lightWidth.h() - (haveBottom ? lightWidth.h() / 2 : 0));
	l.push_back(Layer(/*-surroundMargin*/ - lightMargin, false, true));
	setLayers(l);
}

void DirectionPickerBody::draw(Context const& _c, unsigned _l)
{
	iSize surroundWidth = GUIApp::joint().display->toPixels(fSize(c_surroundWidth, c_surroundWidth));
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));

	bool haveLeft = m_grouping & ForceLeft;
	bool haveRight = m_grouping & ForceRight;
	bool haveTop = m_grouping & ForceAbove;
	bool haveBottom = m_grouping & ForceBelow;

	iRect surroundPx = rect();
	iMargin surroundMargin(haveLeft ? 0 : surroundWidth.w(), haveTop ? 0 : surroundWidth.h(), haveRight ? 0 : surroundWidth.w(), haveBottom ? 0 : surroundWidth.h());
	iMargin lightMargin(lightWidth.w() / (haveLeft ? 2 : 1), lightWidth.h() / (haveTop ? 2 : 1), lightWidth.w() - (haveRight ? lightWidth.w() / 2 : 0), lightWidth.h() - (haveBottom ? lightWidth.h() / 2 : 0));
	iRect outerPx = surroundPx.inset(surroundMargin);
	iRect innerPx = outerPx.inset(lightMargin);
	iRect activePx = innerPx.inset(lightMargin);

	fRect activeMM(GUIApp::joint().display->fromPixels(activePx));
	Color glow = GUIApp::joint().glow(m_color);

	if (_l == 0)
	{
		for (int i = 0; i < 9; ++i)
			if (i % 4)
			{
				_c.xRule(activeMM, i / 8.f, 2, m_color.attenuated(.25f));
				_c.yRule(activeMM, i / 8.f, 2, m_color.attenuated(.25f));
			}

		_c.xRule(activeMM, .5f, 2, m_color.attenuated(.5f));
		_c.yRule(activeMM, .5f, 2, m_color.attenuated(.5f));

		if (m_mode == Fill)
			_c.disc(m_direction.transformedInto(activeMM), Color(1.f, .5f));

		_c.rectOutline(outerPx, surroundMargin, Color(0));
		_c.rectOutline(innerPx, lightMargin, Color(m_color.hue(), m_color.sat(), .125f));
	}
	else if (_l == 1)
	{
		_c.rectOutline(innerPx.outset(lightWidth / 4), iMargin(lightWidth / 2), glow);
	}
	else if (_l == 2 || _l == 3)
	{
		if (m_mode == Circle || m_mode == Fill)
			_c.circle(m_direction.transformedInto(activeMM), m_mode == Circle ? 2.f : 1.f, glow);

		if (m_mode >= Circle)
		{
			_c.disc(fEllipse(activeMM.lerp(xC(-1), yC(-1)), GUIApp::style().thumbDiameter / 8), glow);
			_c.disc(fEllipse(activeMM.lerp(xC(-1), yC(1)), GUIApp::style().thumbDiameter / 8), glow);
			_c.disc(fEllipse(activeMM.lerp(xC(1), yC(-1)), GUIApp::style().thumbDiameter / 8), glow);
			_c.disc(fEllipse(activeMM.lerp(xC(1), yC(1)), GUIApp::style().thumbDiameter / 8), glow);
		}

		if (m_mode == Circle || m_mode == Fill)
			_c.blitThumb(activeMM.lerp(xC(m_lastSign.w()), yC(m_lastSign.h())), m_color, 1.f);
		_c.blitThumb(activeMM.lerp(m_direction.pos()), m_color, 1.f);
	}
}

fSize DirectionPickerBody::specifyFit(fSize _space) const
{
	float s = min(_space.w(), _space.h()) * 1.25f;
	return fSize(min(_space.w(), s), min(_space.h(), s));
}


