#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "DirectionPicker.h"
using namespace std;
using namespace Lightbox;

DirectionPickerBody::DirectionPickerBody(Color _c):
	m_direction(0.5f, 0.5f, 0.5f),
	m_mode(Point),
	m_color(_c),
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

void DirectionPickerBody::draw(Context const& _c)
{
	iSize surroundWidth = GUIApp::joint().display->toPixels(fSize(c_surroundWidth, c_surroundWidth));
	iSize lightWidth = GUIApp::joint().display->toPixels(fSize(c_lightWidth, c_lightWidth));

	iRect surround = rect();
	iMargin surroundMargin(surroundWidth);
	iMargin lightMargin(lightWidth);
	iRect outer = surround.inset(surroundMargin);
	iRect enner = outer.inset(lightMargin);
	_c.rectOutline(outer, surroundMargin, Color(0));
	_c.rectOutline(enner, lightMargin, Color(m_color.hue(), m_color.sat(), .125f));
	_c.rectOutline(enner.outset(lightWidth / 4), iMargin(lightWidth / 2), m_color);

	static const fSize c_thumbMM(20, 20);
	fRect inner(GUIApp::joint().display->fromPixels(enner));

	// m_radius [0, 1]
	// m_direction [0, 1]

	for (int i = 0; i < 9; ++i)
		if (i % 4)
		{
			_c.xRule(inner, i / 8.f, 2, m_color.attenuated(.25f));
			_c.yRule(inner, i / 8.f, 2, m_color.attenuated(.25f));
		}

	_c.xRule(inner, .5f, 2, m_color.attenuated(.5f));
	_c.yRule(inner, .5f, 2, m_color.attenuated(.5f));

	if (m_mode == Circle || m_mode == Fill)
	{
		_c.circle(m_direction.transformedInto(inner), White);
		_c.disc(inner.lerp(xC(m_lastSign.w()), yC(m_lastSign.h())), c_thumbMM, White);
	}

	if (m_mode == Fill)
		_c.disc(m_direction.transformedInto(inner), Color(1.f, .5f));

	if (m_mode >= Circle)
	{
		_c.disc(inner.lerp(xC(-1), yC(-1)), c_thumbMM / 4, White);
		_c.disc(inner.lerp(xC(-1), yC(1)), c_thumbMM / 4, White);
		_c.disc(inner.lerp(xC(1), yC(-1)), c_thumbMM / 4, White);
		_c.disc(inner.lerp(xC(1), yC(1)), c_thumbMM / 4, White);
	}

	_c.disc(inner.lerp(m_direction.pos()), c_thumbMM, White);
}

fSize DirectionPickerBody::specifyFit(fSize _space) const
{
	float s = min(_space.w(), _space.h());
	return fSize(s, s);
}


