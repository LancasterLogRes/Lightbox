#include <array>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "GUIApp.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

void BasicButtonBody::draw(Context _c)
{
	auto const& j = GUIApp::joint();

	j.offsetScale = fRect(m_geometry).translated(_c.offset).asVector4();
	j.color = m_isDown ? GUIApp::style().high.toRGBA() : GUIApp::style().back.toRGBA();

	{
		ProgramUser u(j.program);
		j.geometry.setData(j.unitQuad, 2);
		u.triangleStrip(4);
	}

	auto const& f = m_isDown ? GUIApp::style().bold : GUIApp::style().regular;
	f.draw(m_geometry.lerp(.5f, .5f) + fSize(1, -1), m_text, RGBA(0.f, 0.f, 0.f, .9f));
	f.draw(m_geometry.lerp(.5f, .5f), m_text, RGBA(GUIApp::style().fore * 2.f));
}

bool BasicButtonBody::event(Event* _e)
{
	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
//		cdebug << boolalpha << e->id << "DOWN at" << e->local << m_geometry.contains(e->local);
		m_isDown = true;
		m_downPointer = e->id;
		lockPointer(e->id);
		update();
		return true;
	}
	else if (auto e = dynamic_cast<TouchUpEvent*>(_e))
	{
//		cdebug << boolalpha << e->id << "UP at" << e->local << m_isDown << (e->id == m_downPointer) << m_geometry.contains(e->local);
		if (m_isDown && e->id == m_downPointer)
		{
			if (m_geometry.contains(e->local))
				tapped();
			m_downPointer = -1;
			m_isDown = false;
			update();
		}
		return true;
	}
	return false;
}

void BasicButtonBody::tapped()
{
	if (m_onTapped)
		m_onTapped(view());
}
