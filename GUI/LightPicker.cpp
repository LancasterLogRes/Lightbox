#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Slate.h"
#include "LightPicker.h"
using namespace std;
using namespace lb;

LightPickerBody::LightPickerBody():
	m_light(.5f),
	m_middle(-1.f, 0.f, .5f)
{
}

LightPickerBody::~LightPickerBody()
{
}

void LightPickerBody::initGraphics()
{
	m_lightBar = Program("LightBar.glsl");
	m_lightBar.tie(GUIApp::joint().uniforms);
	setLayers({{Layer(), Layer(Premultiplied)}});
}

void LightPickerBody::finiGraphics()
{
	m_lightBar = Program();
}

bool LightPickerBody::event(Event* _e)
{
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (dynamic_cast<TouchDownEvent*>(e))
		lockPointer(e->id);
	if (e && pointerLocked(e->id))
	{
		fRect geo = geometry().inset(0.f, GUIApp::style().thumbDiameter.h() / 2 + 2);
		setLight(clamp(lext(e->mmLocal().y(), geo.bottom(), geo.top()), 0.f, 1.f), true);
		return true;
	}
	return Super::event(_e);
}

static const float c_outerMargin = 5;
static const float c_innerMM = 10;

void LightPickerBody::draw(Slate const& _c, unsigned _l)
{
	fRect fullOuter = fRect(fSize(GUIApp::style().thumbDiameter.w() + c_outerMargin * 2, geometry().h()));
	fRect outer = fullOuter.inset(fSize(c_outerMargin));
	fRect inner = outer.inset((GUIApp::style().thumbDiameter.w() - c_innerMM) / 2, GUIApp::style().thumbDiameter.h() / 2);
	if (_l == 0)
	{
		ProgramUser u(m_lightBar);
		auto top = u.uniform("topColor");
		auto bottom = u.uniform("bottomColor");
		RGBA rgba(Color(HSLSpace, m_middle.hue(), 1, 0.5f));

		top = RGBA(1.f, 1.f, 1.f, 1.f);
		bottom = rgba;
		_c.mmRect(inner.lerp(0, 0, 1, .5f));

		top = rgba;
		bottom = RGBA(0.f, 0.f, 0.f, 1.f);
		_c.mmRect(inner.lerp(0, 0.5f, 1, 1));
	}
	else
		_c.mmGlowThumb(inner.lerp(.5f, 1.f - m_light), Color(HSLSpace, m_middle.hue(), 1, m_light));
}

fSize LightPickerBody::specifyFit(fSize _space) const
{
	return fSize(max(_space.width(), GUIApp::style().thumbDiameter.width() + c_outerMargin * 2), max(_space.height(), GUIApp::style().thumbDiameter.height() + c_outerMargin * 2));
}
