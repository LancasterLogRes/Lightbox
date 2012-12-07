#include <Common/Global.h>
#include <LGL.h>
#include "Global.h"
#include "GUIApp.h"
#include "Shaders.h"
#include "HuePicker.h"
using namespace std;
using namespace Lightbox;

HuePickerBody::HuePickerBody()
{
	m_hueWheel = Program(Shader::vertex(LB_R(HueWheel_vert)), Shader::fragment(LB_R(HueWheel_frag)));
	m_hueWheel.tie(GUIApp::joint().uniforms);
}

HuePickerBody::~HuePickerBody()
{
}

bool HuePickerBody::event(Event* _e)
{
	if (TouchEvent* e = dynamic_cast<TouchEvent*>(_e))
	{
		if (dynamic_cast<TouchDownEvent*>(_e))
			lockPointer(e->id);
		float s = min(geometry().w(), geometry().h());
		fVector2 off = e->local - geometry().lerp(.5f, .5f);
		if (off.length() < s && off.length() > s / 8.5f)
		{
			setChecked(true);
			setHue(atan2(off.x(), off.y()) / TwoPi + 0.5f);
			return true;
		}
	}
	return Super::event(_e);
}

void HuePickerBody::draw(Context const& _c)
{
	float s = min(geometry().w(), geometry().h());
	fRect geo(geometry().pos() + (geometry().size() - fSize(s, s)) / 2.f, fSize(s, s));

	_c.disc(geo.lerp(.5f, .5f), s / 2.f, m_hueWheel);
	_c.disc(geo.lerp(.5f, .5f), s / 8.f, Color(0.f));
	if (isChecked())
		_c.disc(geo.lerp(.5f, .5f), s / 9.f, Color(m_hue, 1.f, 1.f));
}

fSize HuePickerBody::specifyMinimumSize() const
{
	return Super::specifyMinimumSize();
}

fSize HuePickerBody::specifyMaximumSize() const
{
	return Super::specifyMaximumSize();
}
