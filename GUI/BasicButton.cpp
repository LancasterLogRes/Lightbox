#include <array>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "GUIApp.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

BasicButtonBody::BasicButtonBody(std::string const& _text, Color _c):
	m_text(_text),
	m_color(_c),
	m_isDown(false)
{
}

void BasicButtonBody::draw(Context const& _c)
{
	iRect surround = _c.pixels(this);
	iRect outer = surround.inset(_c.pixels(fSize(2, 2)));
	iRect inner = outer.inset(_c.pixels(fSize(4, 4)));
	_c.rect(surround, Color(0));
	_c.rect(outer, m_isDown ? m_color : Color(m_color.hue(), .25f, .25f));
	_c.rect(inner, Color(.125f), -.1f);
	_c.text(GUIApp::style().bold, inner.lerp(.5f, .5f), m_text, (m_isDown ? m_color : Color(m_color.hue(), .125f, .625f)).toRGBA());
}

bool BasicButtonBody::event(Event* _e)
{
	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
		cdebug << m_text << ":" << boolalpha << e->id << "DOWN at" << e->local << geometry().contains(e->local);
		m_isDown = true;
		m_downPointer = e->id;
		lockPointer(e->id);
		update();
		return true;
	}
	else if (auto e = dynamic_cast<TouchUpEvent*>(_e))
	{
		cdebug << m_text << ":" << boolalpha << e->id << "UP at" << e->local << m_isDown << (e->id == m_downPointer) << geometry().contains(e->local);
		if (m_isDown && e->id == m_downPointer)
		{
			if (geometry().contains(e->local))
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
		m_onTapped(this);
}

fSize BasicButtonBody::specifyMinimumSize(fSize) const
{
	// Until Fonts work without renderer.
	return GUIApp::get() ? GUIApp::style().bold.measure(m_text) : fSize(0, 0);
}
