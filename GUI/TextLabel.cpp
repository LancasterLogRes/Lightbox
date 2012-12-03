#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "TextLabel.h"
using namespace std;
using namespace Lightbox;

TextLabelBody::TextLabelBody(std::string _text): m_text(_text), m_font(), m_color(NullColor)
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
	auto transGeo = m_geometry.translated(_c.offset);
	auto const& f = m_font.isValid() ? m_font : GUIApp::style().regular;
	f.draw(transGeo.lerp(.5f, .5f), m_text, RGBA(m_color.isValid() ? m_color : GUIApp::style().fore));
}

fSize TextLabelBody::minimumSize() const
{
	auto const& f = m_font.isValid() ? m_font : GUIApp::style().regular;
	return f.measure(m_text);
}
