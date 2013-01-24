#include <Common/Global.h>
#include <LGL.h>
#include "Global.h"
#include "GUIApp.h"
#include "Shaders.h"
#include "HuePicker.h"
using namespace std;
using namespace Lightbox;

HuePickerBody::HuePickerBody(Color _c, Grouping _grouping): ViewCreator(string(), _c, _grouping)
{
}

HuePickerBody::~HuePickerBody()
{
}

void HuePickerBody::initGraphics()
{
	m_hueWheel = Program(Shader::vertex(LB_R(HueWheel_vert)), Shader::fragment(LB_R(HueWheel_frag)));
	m_hueWheel.tie(GUIApp::joint().uniforms);
}

void HuePickerBody::finiGraphics()
{
	m_hueWheel = Program();
}

bool HuePickerBody::event(Event* _e)
{
	bool ret = Super::event(_e);
	TouchEvent* e = dynamic_cast<TouchEvent*>(_e);
	if (e && pointerLocked(e->id))
	{
		float s = min(geometry().w(), geometry().h());
		fVector2 off = e->local - geometry().lerp(.5f, .5f);
		if (off.length() < s && off.length() > s / 8.5f)
		{
			setChecked(true, true);
			setHue(atan2(off.x(), off.y()) / TwoPi + 0.5f, true);
			return true;
		}
	}
	return ret;
}

void HuePickerBody::draw(Context const& _c, unsigned _l)
{
	BasicButtonBody::drawButton(_c, _l, isChecked(), isDown() || isChecked(),
	[&](iRect inner){
		iEllipse e(inner);
		{
			ProgramUser u(m_hueWheel);
			_c.disc(e);
		}
		_c.disc(e.inset(e.radii() * 6 / 10), Color(0.f));
		if (isChecked())
			_c.disc(e.inset(e.radii() * 7 / 10), Color(m_hue, 1.f, 1.f));
	}, false);
	return;
}

fSize HuePickerBody::specifyMinimumSize(fSize _s) const
{
	return Super::specifyMinimumSize(_s);
}

fSize HuePickerBody::specifyMaximumSize(fSize _s) const
{
	return Super::specifyMaximumSize(_s);
}

fSize HuePickerBody::specifyFit(fSize _space) const
{
	float s = min(_space.w(), _space.h());
	return fSize(s, s);
}
