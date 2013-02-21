#include <Common/Global.h>
#include "GUIApp.h"
#include "Slate.h"
#include "Global.h"
#include "MarkdownView.h"
using namespace std;
using namespace Lightbox;

MarkdownViewBody::MarkdownViewBody(std::string const& _text, Color _color, Font const& _font):
	m_text(_text),
	m_font(_font),
	m_color(_color)
{
}

MarkdownViewBody::~MarkdownViewBody()
{
}

bool MarkdownViewBody::event(Event* _e)
{
	return Super::event(_e);
}

void MarkdownViewBody::draw(Slate const& _c, unsigned)
{
	// Wrap text.
	iRect r = rect();
	Font f = m_font.isValid() ? m_font : GUIApp::style().small;
	Color c = m_color.isValid() ? m_color : GUIApp::style().fore;

	iCoord cursor = rect().pos();
	for (unsigned i = 0; i < m_text.size() && cursor.y() < r.bottom();)
	{
		// Find word-breaks for potential new-lines or '\n' character.
		unsigned breakpoint = i;
		for (unsigned j = i; j < m_text.size(); ++j)
			if (m_text[j] == '\n' || m_text[j] == ' ')
			{
				// if string too long, break with last breakpoint, else continue noting current breakpoint.
				fRect l = f.measurePx(m_text.substr(i, j - i));
				if (l.w() > r.w() && breakpoint != i)
					break;
				breakpoint = j;
				if (m_text[j] == '\n')
					break;
			}
			else if (j == m_text.size() - 1)
			{
				breakpoint = m_text.size();
			}
		string t = m_text.substr(i, breakpoint - i);
		iRect r = (iRect)f.measurePx(t);
		_c.text(f, cursor, t, c, AtLeft|AtTop);
		cursor.setY(cursor.y() + r.h());
		i = breakpoint + 1;
	}
}
