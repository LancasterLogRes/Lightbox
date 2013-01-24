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

static const fSize c_thumbMM(20, 20);
static const float c_thumbOutlineMM = 1;
static const float c_innerMM = 10;

void LightPickerBody::draw(Context const& _c)
{
	fRect outer = fRect(fSize(c_thumbMM.width(), geometry().height()));
	fRect inner = outer.inset((c_thumbMM.w() - c_innerMM) / 2, c_thumbMM.h() / 2);
	{
		ProgramUser u(m_lightBar);
		auto top = u.uniform("topColor");
		auto bottom = u.uniform("bottomColor");

		top = RGBA(1.f, 1.f, 1.f, 1.f);
		bottom = RGBA(m_middle);
		_c.rect(inner.lerp(0, 0, 1, .5f));

		top = RGBA(m_middle);
		bottom = RGBA(0.f, 0.f, 0.f, 1.f);
		_c.rect(inner.lerp(0, 0.5f, 1, 1));
	}

	fEllipse thumb(inner.lerp(.5f, 1.f - m_light), c_thumbMM / 2);
	_c.disc(thumb, White);

	Color c = Color(m_middle.hue(), m_middle.sat() == 0.f ? 0.f : clamp(2.f - m_light * 2.f, 0.f, 1.f), m_middle.sat() == 0.f ? m_light : clamp(m_light * 2.f, 0.f, 1.f));
	_c.disc(thumb.inset(c_thumbOutlineMM), c);
}

fSize LightPickerBody::specifyFit(fSize _space) const
{
	return fSize(c_thumbMM.width(), _space.height());
}
