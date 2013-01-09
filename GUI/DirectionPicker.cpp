#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "DirectionPicker.h"
using namespace std;
using namespace Lightbox;

DirectionPickerBody::DirectionPickerBody():
	m_direction(0.5f, 0.5f, 0.5f),
	m_mode(Point)
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
				fSize r(min(1.f, wC(p.x())), min(1.f, hC(p.y())));
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

void DirectionPickerBody::draw(Context const& _c)
{
	// m_radius [0, 1]
	// m_direction [0, 1]
	float s = geometry().size().max();
	if (m_mode == Circle || m_mode == Fill)
	{
		_c.circle(m_direction * geometry().size(), White);
		_c.disc(fCoord(xC(m_lastSign.w()), yC(m_lastSign.h())) * geometry().size(), .1f * s, White);
	}

	if (m_mode == Fill)
		_c.disc(m_direction * geometry().size(), Color(1.f, .5f));

	if (m_mode >= Circle)
	{
		_c.disc(fCoord(xC(-1), yC(-1)) * geometry().size(), .025f * s, White);
		_c.disc(fCoord(xC(-1), yC(1)) * geometry().size(), .025f * s, White);
		_c.disc(fCoord(xC(1), yC(-1)) * geometry().size(), .025f * s, White);
		_c.disc(fCoord(xC(1), yC(1)) * geometry().size(), .025f * s, White);
	}

	_c.disc(m_direction.pos() * geometry().size(), s / 10.f, Color(1.f));
}

fSize DirectionPickerBody::specifyFit(fSize _space) const
{
	float s = min(_space.w(), _space.h());
	return fSize(s, s);
}


