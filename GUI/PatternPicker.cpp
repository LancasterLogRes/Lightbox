#include <Common/Global.h>
#include "Global.h"
#include "PatternPicker.h"
using namespace std;
using namespace Lightbox;

PatternPickerBody::PatternPickerBody(Color _c, Grouping _grouping):
	ViewCreator(string(), _c, _grouping),
	m_index(0),
	m_space(12, 12)
{
}

PatternPickerBody::~PatternPickerBody()
{
}

bool PatternPickerBody::event(Event* _e)
{
	bool ret = Super::event(_e);
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (e && pointerLocked(e->id))
	{
		iCoord place = (iCoord)fCoord(fSize(e->local - geometry().pos()) / fSize(geometry().size()) * fSize(m_space));
		setChecked(true, true);
		setIndex(clamp<int>(place.x(), 0u, m_space.w() - 1) + clamp<int>(place.y(), 0u, m_space.h() - 1) * m_space.w(), true);
		return true;
	}
	return ret;
}

void PatternPickerBody::draw(Context const& _c, unsigned _layer)
{
	drawButton(_c, _layer, isChecked(),
	[&](iRect oinner)
	{
		fSize thumbOutPx = _c.pixelsF(GUIApp::style().thumbSize / 2 + GUIApp::style().thumbOutline);
		fSize thumbPx = _c.pixelsF(GUIApp::style().thumbSize / 2);
		fRect inner = fRect(oinner).inset(thumbPx);

		fSize spacing(inner.size() / ((fSize)m_space - fSize(1, 1)));
		Color glow = Color(color().hue(), color().sat() * .95f, color().value() * 8.f, lerp(color().sat(), .65f, .75f));
		int i = 0;
		for (unsigned y = 0; y < m_space.h(); ++y)
			for (unsigned x = 0; x < m_space.w(); ++x, ++i)
			{
				fCoord p = inner.pos() + fSize(x, y) * spacing;
				if (isChecked() && m_index == i)
					if (_layer == 1)
					{
						_c.pxDisc(fEllipse(p, thumbOutPx), GUIApp::style().outlineColor);
						_c.pxDisc(fEllipse(p, thumbPx), glow);
					}
					else {}
				else if (_layer == 0)
					_c.pxRect(fRect::square(p, min(spacing.w(), spacing.h()) / 6.f), glow.attenuated(.25f));
			}
	}, false);
}


