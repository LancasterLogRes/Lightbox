#include <Common/Global.h>
#include "Global.h"
#include "PatternPicker.h"
using namespace std;
using namespace Lightbox;

PatternPickerBody::PatternPickerBody():
	m_index(0),
	m_space(12, 12)
{
}

PatternPickerBody::~PatternPickerBody()
{
}

bool PatternPickerBody::event(Event* _e)
{
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (dynamic_cast<TouchDownEvent*>(e))
		lockPointer(e->id);
	if (e && pointerLocked(e->id))
	{
		iCoord place = (iCoord)fCoord(fSize(e->local - geometry().pos()) / fSize(geometry().size()) * fSize(m_space));
		setChecked(true);
		setIndex(clamp<int>(place.x(), 0u, m_space.w() - 1) + clamp<int>(place.y(), 0u, m_space.h() - 1) * m_space.w());
		return true;
	}
	return Super::event(_e);
}

bool PatternPickerBody::draw(Context const& _c)
{
	fSize spacing(geometry().size() / fSize(m_space));
	int i = 0;
	for (unsigned y = 0; y < m_space.h(); ++y)
		for (unsigned x = 0; x < m_space.w(); ++x, ++i)
		{
			fCoord p = spacing * fCoord(x + .5f, y + .5f);
			if (isChecked() && m_index == i)
				_c.disc(p, min(spacing.w(), spacing.h()) / 1.5f, Color(1.f));
			else
				_c.rect(fRect::square(p, min(spacing.w(), spacing.h()) / 6.f), Color(0.25f));
		}
	return true;
}


