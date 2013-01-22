#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "Shaders.h"
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
	m_lightBar = Program(Shader::vertex(LB_R(LightBar_vert)), Shader::fragment(LB_R(HueWheel_frag)));
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

void LightPickerBody::draw(Context const& _c)
{
	fRect geo = geometry();
	{
		ProgramUser u(m_lightBar);
		auto top = u.uniform("topColor");
		auto bottom = u.uniform("bottomColor");

		top = RGBA(1.f, 1.f, 1.f, 1.f);
		bottom = RGBA(m_middle);
		_c.rect(fRect(geo.pos(), fSize(geo.width(), geo.height() / 2.f)));

		top = RGBA(m_middle);
		bottom = RGBA(0.f, 0.f, 0.f, 1.f);
		_c.rect(fRect(geo.lerp(0.f, .5f), fSize(geo.width(), geo.height() / 2.f)));
	}
	Color c = Color(m_middle.hue(), m_middle.sat() == 0.f ? 0.f : clamp(2.f - m_light * 2.f, 0.f, 1.f), m_middle.sat() == 0.f ? m_light : clamp(m_light * 2.f, 0.f, 1.f));
	geo = geo.inset(0.f, geo.w() / 2 + 2);
	_c.rect(fRect(geo.lerp(0.f, 1.f - m_light) - fSize(0.f, geo.w() / 2 + 2), fSize(geo.w(), geo.w() + 5)), Black);
	_c.rect(fRect(geo.lerp(0.f, 1.f - m_light) - fSize(0.f, geo.w() / 2 + 1), fSize(geo.w(), geo.w() + 3)), White);
	_c.rect(fRect(geo.lerp(0.f, 1.f - m_light) - fSize(0.f, geo.w() / 2), fSize(geo.w(), geo.w() + 1)), c);
}

fSize LightPickerBody::specifyFit(fSize _space) const
{
	return Super::specifyFit(_space);
	float w = min(_space.w(), _space.h() / 6);
	return fSize(w, w * 6);
}
