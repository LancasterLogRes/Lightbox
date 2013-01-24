#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "TextLabel.h"
using namespace std;
using namespace Lightbox;

TextLabelBody::TextLabelBody(std::string const& _text, Color _color, Font const& _font): m_text(_text), m_font(_font), m_color(_color), m_rule(false)
{
}

TextLabelBody::~TextLabelBody()
{
}

bool TextLabelBody::event(Event* _e)
{
	return Super::event(_e);
}

void TextLabelBody::draw(Context const& _c)
{
	iRect geo = rect();
	Font f = m_font.isValid() ? m_font : GUIApp::style().regular;
	Color c = m_color.isValid() ? m_color : GUIApp::style().fore;
	_c.text(f, geo.lerp(.5f, .5f), m_text, RGBA(c));
	int w = (GUIApp::joint().display->toPixels(fRect(f.measure(m_text) + fSize(16, 0)))).width() / 2;
	if (m_rule)
	{
		_c.rect(geo.lerp(0, .5f, .5f, .5f).outset(0, 1, 0, 0).inset(0, 0, w, 0), c);
		_c.rect(geo.lerp(.5f, .5f, 1.f, .5f).outset(0, 1, 0, 0).inset(w, 0, 0, 0), c);
	}
}

fSize TextLabelBody::specifyMinimumSize(fSize) const
{
	return GUIApp::get() ? (m_font.isValid() ? m_font : GUIApp::style().small).measure(m_text) : fSize(0, 0);
}
