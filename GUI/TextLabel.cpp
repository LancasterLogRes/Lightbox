#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "TextLabel.h"
using namespace std;
using namespace Lightbox;

TextLabelBody::TextLabelBody(std::string const& _text, Color _color, Font const& _font): m_text(_text), m_font(_font), m_color(_color)
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
	fRect transGeo(fCoord(0, 0), geometry().size());
	_c.text(m_font.isValid() ? m_font : GUIApp::style().regular, transGeo.lerp(.5f, .5f), m_text, RGBA(m_color.isValid() ? m_color : GUIApp::style().fore));
}

fSize TextLabelBody::specifyMinimumSize(fSize) const
{
	auto const& f = m_font.isValid() ? m_font : GUIApp::style().regular;
	return f.measure(m_text);
}
