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
	m_lightBar = Program(Shader::vertex(LB_R(LightBar_vert)), Shader::fragment(LB_R(HueWheel_frag)));
	m_lightBar.tie(GUIApp::joint().uniforms);
}

LightPickerBody::~LightPickerBody()
{
}

bool LightPickerBody::event(Event* _e)
{
	if (TouchEvent* e = dynamic_cast<TouchEvent*>(_e))
	{
		if (dynamic_cast<TouchDownEvent*>(_e))
			lockPointer(e->id);
		setLight(clamp(lext(e->local.y(), geometry().bottom(), geometry().top()), 0.f, 1.f));
		return true;
	}
	return Super::event(_e);
}

void LightPickerBody::draw(Context const& _c)
{
	ProgramUser u(m_lightBar);
	auto top = u.uniform("topColor");
	auto bottom = u.uniform("bottomColor");
	fRect geo = geometry();

	top = RGBA(1.f, 1.f, 1.f, 1.f);
	bottom = RGBA(m_middle);
	_c.rect(fRect(geo.pos(), fSize(geo.width(), geo.height() / 2.f)));

	top = RGBA(m_middle);
	bottom = RGBA(0.f, 0.f, 0.f, 1.f);
	_c.rect(fRect(geo.lerp(0.f, .5f), fSize(geo.width(), geo.height() / 2.f)));

	Color c = Color(m_middle.hue(), m_middle.sat() == 0.f ? 0.f : clamp(2.f - m_light * 2.f, 0.f, 1.f), m_middle.sat() == 0.f ? m_light : clamp(m_light * 2.f, 0.f, 1.f));
	_c.rect(fRect(geo.lerp(0.f, 1.f - m_light) - fSize(0.f, 5.f), fSize(geo.w(), 11.f)), Black);
	_c.rect(fRect(geo.lerp(0.f, 1.f - m_light) - fSize(0.f, 3.f), fSize(geo.w(), 7.f)), c);
}


