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
	float f = sqrt((geometry().w() - GUIApp::style().thumbDiameter.w()) * (geometry().h() - GUIApp::style().thumbDiameter.h()) / 128.f);
	m_space = uSize(ceil(geometry().w() / f), ceil(geometry().h() / f));
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
		setIndex(clamp<int>(place.x(), 0u, m_space.w() - 1) + clamp<int>(place.y(), 0u, m_space.h() - 1) * m_space.w(), true);
		return true;
	}
	return ret;
}

void PatternPickerBody::initGraphics()
{
	Super::initGraphics();
	Layers l = layers();
	l.push_back(Layer(iMargin(), false, true));
	setLayers(l);
}

void PatternPickerBody::updateLayers()
{
	Super::updateLayers();
	layer(2).show(isEnabled() && isLit());
}

void PatternPickerBody::draw(Context const& _c, unsigned _layer)
{
	unsigned xBig[] = { m_space.w() / 4, m_space.w() * 3 / 4 };
	unsigned yBig[] = { m_space.h() / 4, m_space.h() * 3 / 4 };
	drawButton(_c, _layer, isChecked(),
	[&](iRect oinner)
	{
		fSize thumbOutPx = _c.pixelsF(GUIApp::style().thumbDiameter / 2 + GUIApp::style().thumbOutline);
		fSize thumbPx = _c.pixelsF(GUIApp::style().thumbDiameter / 2);
		fRect inner = fRect(oinner).inset(thumbPx);

		fSize spacing(inner.size() / ((fSize)m_space - fSize(1, 1)));
		Color glow = Color(color().hue(), color().sat() * .95f, color().value() * 8.f, lerp(color().sat(), .65f, .75f));
		int i = 0;
		for (unsigned y = 0; y < m_space.h(); ++y)
			for (unsigned x = 0; x < m_space.w(); ++x, ++i)
			{
				fCoord p = inner.pos() + fSize(x, y) * spacing;
				if (_layer == 0)
					_c.pxRect(fRect::square(p, min(spacing.w(), spacing.h()) / ((x == xBig[0] || x == xBig[1]) && (y == yBig[0] || y == yBig[1]) ? 3.f : 6.f)), glow.attenuated(.25f));
				else if (isChecked() && m_index == i && _layer == 2)
					_c.blitThumb(iCoord(p), color(), 1.f);
			}
	}, false);
}


