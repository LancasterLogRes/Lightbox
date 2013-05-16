#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "Slate.h"
#include "BasicButton.h"
#include "DirectionPicker.h"
using namespace std;
using namespace lb;

DirectionPickerBody::DirectionPickerBody(Color _c, Grouping _grouping):
	m_direction(0.5f, 0.5f, 0.5f),
	m_mode(Point),
	m_color(_c),
	m_grouping(_grouping),
	m_enableH(true),
	m_enableV(true),
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
		fCoord p = fCoord(fSize(e->mmLocal() - geometry().pos()) / fSize(geometry().size())).clamped(0.f, 1.f);
		if (!m_enableH)
			p.setX(0);
		if (!m_enableV)
			p.setY(0);
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
			return true;
		}
	}
	return Super::event(_e);
}

void DirectionPickerBody::initGraphics()
{
	setLayers({{ Layer(), Layer(borderMargin(), Premultiplied), Layer(-innerMargin(m_grouping) / 2, Premultiplied) }});
}

void DirectionPickerBody::draw(Slate const& _c, unsigned _l)
{
	iRect inner = rect().inset(innerMargin(m_grouping));
	drawButton(_c, inner, m_color, true, _l == 0, _l == 1, false);

	iRect activePx = inner.inset(GUIApp::joint().lightEdgePixels);
	fRect activeMm(GUIApp::joint().display->fromPixels(activePx));

	if (_l == 0)
	{
		for (int i = 0; i < 9; ++i)
			if (i % 4)
			{
				_c.mmXRule(activeMm, i / 8.f, 2, m_color.attenuated(.25f));
				_c.mmYRule(activeMm, i / 8.f, 2, m_color.attenuated(.25f));
			}
		_c.mmXRule(activeMm, .5f, 2, m_color.attenuated(.5f));
		_c.mmYRule(activeMm, .5f, 2, m_color.attenuated(.5f));
	}
	else if (_l == 2)
	{
		if (m_mode == Circle || m_mode == Fill)
			_c.mmCircle(m_direction.transformedInto(activeMm), m_mode == Circle ? 2.f : 1.f, m_color);

		if (m_mode >= Circle)
		{
			_c.mmDisc(fEllipse(activeMm.lerp(xC(-1), yC(-1)), GUIApp::style().thumbDiameter / 8), m_color);
			_c.mmDisc(fEllipse(activeMm.lerp(xC(-1), yC(1)), GUIApp::style().thumbDiameter / 8), m_color);
			_c.mmDisc(fEllipse(activeMm.lerp(xC(1), yC(-1)), GUIApp::style().thumbDiameter / 8), m_color);
			_c.mmDisc(fEllipse(activeMm.lerp(xC(1), yC(1)), GUIApp::style().thumbDiameter / 8), m_color);
		}

		if (m_mode == Fill)
			_c.mmDisc(m_direction.transformedInto(activeMm), Color(1.f, .5f));

		if (m_mode == Circle || m_mode == Fill)
			_c.mmGlowThumb(activeMm.lerp(xC(m_lastSign.w()), yC(m_lastSign.h())), m_color, 1.f);
		_c.mmGlowThumb(activeMm.lerp(m_direction.pos()), m_color, 1.f);
	}
}

fSize DirectionPickerBody::specifyFit(fSize _space) const
{
	float s = min(_space.w(), _space.h()) * 1.25f;
	return fSize(min(_space.w(), s), min(_space.h(), s));
}


