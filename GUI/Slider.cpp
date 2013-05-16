#include <Common/Global.h>
#include "Global.h"
#include "BasicButton.h"
#include "GUIApp.h"
#include "Slate.h"
#include "Slider.h"
using namespace std;
using namespace lb;

SliderBody::SliderBody(Orientation _o, float _value, float _min, float _max):
	m_o		(_o),
	m_min	(_min),
	m_max	(_max),
	m_value	(_value)
{
}

SliderBody::~SliderBody()
{
}

void SliderBody::initGraphics()
{
	setLayers({{ Layer(), Layer(Premultiplied) }});
}

void SliderBody::finiGraphics()
{
}

bool SliderBody::event(Event* _e)
{
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (dynamic_cast<TouchDownEvent*>(e))
		lockPointer(e->id);
	if (e && pointerLocked(e->id))
	{
		if (m_o == Vertical)
		{
			fRect geo = geometry().inset(0.f, GUIApp::style().thumbDiameter.h() / 2 + 2);
			setValue(lerp(lext(e->mmLocal().y(), geo.bottom(), geo.top()), m_min, m_max), true);
		}
		else
		{
			fRect geo = geometry().inset(GUIApp::style().thumbDiameter.w() / 2 + 2, 0);
			setValue(lerp(lext(e->mmLocal().x(), geo.left(), geo.right()), m_min, m_max), true);
		}
		return true;
	}
	return Super::event(_e);
}

static const float c_outerMargin = 10;
static const float c_innerMM = 20;

void SliderBody::draw(Slate const& _c, unsigned _l)
{
	if (m_o == Vertical)
	{
		fRect fullOuter = fRect(fSize(GUIApp::style().thumbDiameter.w() + c_outerMargin * 2, geometry().h()));
		fRect outer = fullOuter.inset(fSize(c_outerMargin));
		fRect inner = outer.inset((GUIApp::style().thumbDiameter.w() - c_innerMM) / 2, GUIApp::style().thumbDiameter.h() / 2);
		if (_l == 0)
			_c.mmRect(inner, White);
		else
			_c.mmGlowThumb(inner.lerp(.5f, 1.f - lext(m_value, m_min, m_max)), White, 1.f);
	}
	else
	{
		fRect fullOuter = fRect(fSize(geometry().w(), GUIApp::style().thumbDiameter.h() + c_outerMargin * 2));
		fRect outer = fullOuter.inset(fSize(c_outerMargin));
		fRect inner = outer.inset(GUIApp::style().thumbDiameter.w() / 2, (GUIApp::style().thumbDiameter.h() - c_innerMM) / 2);
		if (_l == 0)
		{
			lb::drawButton(_c, rect(), White, true, true, false, false, false);
			lb::drawButton(_c, _c.toPixels(inner), White, false, true, false, true, false);
		}
		else
		{
			_c.mmRect(inner.lerp(0, 0, lext(m_value, m_min, m_max), 1), White);
			_c.mmGlowThumb(inner.lerp(lext(m_value, m_min, m_max), .5f), White, 1.f);
		}
	}
}

fSize SliderBody::specifyFit(fSize _space) const
{
	return fSize(max(_space.width(), GUIApp::style().thumbDiameter.width() + c_outerMargin * 2), max(_space.height(), GUIApp::style().thumbDiameter.height() + c_outerMargin * 2));
}
