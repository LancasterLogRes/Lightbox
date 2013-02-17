#include <Common/Global.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "PatternPicker.h"
using namespace std;
using namespace Lightbox;

PatternPickerBody::PatternPickerBody(Color _c, Grouping _grouping):
	ViewCreator(string(), _c, Font(), _grouping),
	m_index(0),
	m_space(12, 12)
{
}

PatternPickerBody::~PatternPickerBody()
{
}

void PatternPickerBody::resized()
{
	Super::resized();
	// want ~ 128 points from exposed geometry.
	fSize available = geometry().size() - GUIApp::style().thumbDiameter;
	float f = sqrt(available.area() / 128.f);
	m_space = iSize(ceil(available.w() / f), ceil(available.h() / f));
	m_index = min<unsigned>(m_index, m_space.w() * m_space.h() - 1);
}

bool PatternPickerBody::event(Event* _e)
{
	bool ret = Super::event(_e);
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (e && pointerLocked(e->id))
	{
		iCoord place = (iCoord)fCoord(fSize(e->local - geometry().pos()) / fSize(geometry().size()) * fSize(m_space));
		setChecked(true, true);
		setIndex(clamp<int>(place.x(), 0, m_space.w() - 1) + clamp<int>(place.y(), 0, m_space.h() - 1) * m_space.w(), true);
		return true;
	}
	return ret;
}

void PatternPickerBody::initGraphics()
{
	Super::initGraphics();
}

void PatternPickerBody::updateLayers()
{
	Super::updateLayers();
}

void PatternPickerBody::draw(Slate const& _c, unsigned _layer)
{
	int xBig[] = { m_space.w() / 4, m_space.w() * 3 / 4 };
	int yBig[] = { m_space.h() / 4, m_space.h() * 3 / 4 };
	iRect oinner = rect().inset(innerMargin(effectiveGrouping()));
	Lightbox::drawButton(_c, oinner, color(), isChecked(), _layer == 0, _layer == 1, false);
	fSize thumbPx = _c.toPixelsF(GUIApp::style().thumbDiameter / 2);
	fRect inner = fRect(oinner).inset(thumbPx);

	fSize spacing(inner.size() / ((fSize)m_space - fSize(1, 1)));
	Color glow = Color(color().hue(), color().sat() * .95f, color().value() * 8.f, lerp(color().sat(), .65f, .75f));
	int i = 0;
	for (int y = 0; y < m_space.h(); ++y)
		for (int x = 0; x < m_space.w(); ++x, ++i)
		{
			fCoord p = inner.pos() + fSize(x, y) * spacing;
			if (_layer == 0)
				_c.rect(fRect::square(p, min(spacing.w(), spacing.h()) / ((x == xBig[0] || x == xBig[1]) && (y == yBig[0] || y == yBig[1]) ? 3.f : 6.f)), glow.attenuated(.25f));
			else if (_layer == 1 && m_index == i)
			{
				_c.glowThumb(iCoord(p), color(), 1.f);
				_c.text(GUIApp::style().bold, iCoord(p), toString(i), Color(0, 0.5));
			}
		}
}


