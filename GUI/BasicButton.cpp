#include <array>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "GUIApp.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

BasicButtonBody::BasicButtonBody(std::string const& _text):
	m_text(_text),
	m_isDown(false)
{
	setPadding(2.f);
}

void BasicButtonBody::draw(Context const& _c)
{
	fRect transGeo(fCoord(0, 0), geometry().size());
	_c.rect(transGeo, m_isDown ? GUIApp::style().high : GUIApp::style().back, -.1f);
	_c.text(m_isDown ? GUIApp::style().bold : GUIApp::style().regular, transGeo.lerp(.5f, .5f) + fSize(0, -1), m_text, RGBA(0.f, 0.f, 0.f));
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
