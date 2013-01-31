#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "LightBar.h"
#include "LightPicker.h"
using namespace std;
using namespace Lightbox;

LightPickerBody::LightPickerBody():
	m_light(.5f),
	m_middle(0.f, 0.f, .5f)
{
}

LightPickerBody::~LightPickerBody()
{
}

void LightPickerBody::initGraphics()
{
	m_lightBar = LB_PROGRAM(LightBar_glsl, lightbar);
	m_lightBar.tie(GUIApp::joint().uniforms);
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
		fRect geo = geometry().inset(0.f, geometry().w() / 2 + 2);
		setLight(clamp(lext(e->local.y(), geo.bottom(), geo.top()), 0.f, 1.f), true);
		return true;
	}
	return Super::event(_e);
}

static const float c_innerMM = 10;

void LightPickerBody::draw(Context const& _c, unsigned)
{
	fRect outer = fRect(fSize(GUIApp::style().thumbSize.w(), geometry().h()));
	fRect inner = outer.inset((GUIApp::style().thumbSize.w() - c_innerMM) / 2, GUIApp::style().thumbSize.h() / 2);
	{
		ProgramUser u(m_lightBar);
		auto top = u.uniform("topColor");
		auto bottom = u.uniform("bottomColor");
		RGBA rgba(Color(HSLSpace, m_middle.hue(), 1, 0.5f));

		top = RGBA(1.f, 1.f, 1.f, 1.f);
		bottom = rgba;
		_c.rect(inner.lerp(0, 0, 1, .5f));

		top = rgba;
		bottom = RGBA(0.f, 0.f, 0.f, 1.f);
		_c.rect(inner.lerp(0, 0.5f, 1, 1));
	}

	fEllipse thumb(inner.lerp(.5f, 1.f - m_light), GUIApp::style().thumbSize / 2);
	_c.disc(thumb.outset(GUIApp::style().thumbOutline), GUIApp::style().outlineColor);
	_c.disc(thumb, Color(HSLSpace, m_middle.hue(), 1, m_light));
}

fSize LightPickerBody::specifyFit(fSize _space) const
{
	return fSize(GUIApp::style().thumbSize.width(), _space.height());
}
