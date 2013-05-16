#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "Slate.h"
#include "TextLabel.h"
using namespace std;
using namespace lb;

TextLabelBody::TextLabelBody(std::string const& _text, Color _color, Font const& _font, Alignment _a):
	m_text(_text),
	m_font(_font),
	m_color(_color),
	m_rule(false),
	m_alignment(_a)
{
}

TextLabelBody::~TextLabelBody()
{
}

bool TextLabelBody::event(Event* _e)
{
	return Super::event(_e);
}

void TextLabelBody::draw(Slate const& _c, unsigned)
{
	iRect r = rect();
	Font f = m_font.isValid() ? m_font : GUIApp::style().small;
	Color c = m_color.isValid() ? m_color : GUIApp::style().fore;
	iCoord p = r.lerp((m_alignment & AlignLeft) ? 0.f : (m_alignment & AlignRight) ? 1.f : .5f, (m_alignment & AlignTop) ? 0.f : (m_alignment & AlignBottom) ? 1.f : .5f);
	AnchorType a = ((m_alignment & AlignTop) ? AtTop : (m_alignment & AlignBottom) ? AtBottom : (m_alignment & AlignBaselineCenter) ? AtBaseline : AtCenter) | ((m_alignment & AlignLeft) ? AtLeft : (m_alignment & AlignRight) ? AtRight : AtCenter) | ((m_alignment & AlignTightVertical) ? TightVertical : AnchorFlags(0));
	_c.text(f, p, m_text, c, a);
	int w = fRect(f.pxMeasure(m_text).size() + fSize(16, 0)).width() / 2;
	if (m_rule)
	{
		_c.rect(r.lerp(0, .5f, .5f, .5f).outset(0, 1, 0, 0).inset(0, 0, w, 0), c);
		_c.rect(r.lerp(.5f, .5f, 1.f, .5f).outset(0, 1, 0, 0).inset(w, 0, 0, 0), c);
	}
}

fSize TextLabelBody::specifyMinimumSize(fSize) const
{
	return GUIApp::get() ? (m_font.isValid() ? m_font : GUIApp::style().small).measure(m_text).size() : fSize(0, 0);
}
