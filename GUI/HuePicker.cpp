#include <Common/Global.h>
#include <LGL.h>
#include "Global.h"
#include "GUIApp.h"
#include "RenderToTextureSlate.h"
#include "HuePicker.h"
using namespace std;
using namespace Lightbox;

HuePickerBody::HuePickerBody(Color _c, Grouping _grouping):
	ViewCreator(string(), _c, Font(), _grouping),
	m_hue(-1.f),
	m_middle(White)
{
}

HuePickerBody::~HuePickerBody()
{
}

void HuePickerBody::initGraphics()
{
	m_hueWheel = Program("HueWheel.glsl");
	m_hueWheel.tie(GUIApp::joint().uniforms);
	m_hueWheelDodge = Program("HueWheel.glsl", "dodge.vert", "frag");
	m_hueWheelDodge.tie(GUIApp::joint().uniforms);
	Super::initGraphics();
	setLayers({{ layer(0), Layer(iMargin(), true), layer(1) }});
}

void HuePickerBody::finiGraphics()
{
	m_hueWheel = m_hueWheelDodge = Program();
	m_hueWheelTex = Texture2D();
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

void HuePickerBody::updateLayers()
{
	Super::updateLayers();
	layer(2).show(layer(1).isShown());
}

void HuePickerBody::preDraw(unsigned _l)
{
	iSize ts = rect().size();
	if (_l == 1 && m_hueWheelTex.size() != ts)
	{
		m_hueWheelTex = Texture2D(ts);
		RenderToTextureSlate con(m_hueWheelTex);
		ProgramUser u(m_hueWheelDodge);
		iSize thumbPx = con.toPixels(GUIApp::style().thumbDiameter / 2);
		con.disc(iEllipse(rect().inset(innerMargin(effectiveGrouping())).inset(thumbPx)));
	}
}

void HuePickerBody::draw(Slate const& _c, unsigned _l)
{
	iSize thumbPx = _c.toPixels(GUIApp::style().thumbDiameter / 2);
	iRect inner = rect().inset(innerMargin(effectiveGrouping()));
	drawButton(_c, inner, color(), isChecked(), _l == 0, _l == 2, false);
	inner = inner.inset(thumbPx / 2.f);
	iEllipse e(inner);
	if (_l < 2)
	{
		if (_l == 0)
		{
			ProgramUser u(m_hueWheel);
			_c.disc(e.inset(thumbPx / 2));
		}
		else
		{
			_c.blit(m_hueWheelTex);
		}
		_c.disc(e.inset(thumbPx * 3 / 2), Black);
	}
	else
	{
		iCoord p = e.pos() + iSize(fSize(-sin(m_hue * TwoPi), -cos(m_hue * TwoPi)) * fSize(e.radii() - thumbPx));
		_c.glowThumb(p, m_middle.withHue(m_hue));
	}
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
