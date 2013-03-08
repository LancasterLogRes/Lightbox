#pragma once

#include <Common/Color.h>
#include "Font.h"
#include "View.h"

namespace Lightbox
{

class MarkdownViewBody;
typedef boost::intrusive_ptr<MarkdownViewBody> MarkdownView;

enum class RichTextNodeType
{
	Text,
	FontWeight,
	FontFace,
	Color
};

struct RichTextNode
{
	RichTextNodeType type;
	std::string data;
};

class MarkdownViewBody: public ViewCreator<ViewBody, MarkdownViewBody>
{
	friend class ViewBody;

public:
	virtual ~MarkdownViewBody();

	std::string const& text() const { return m_text; }
	Font const& font() const { return m_font; }
	Color const& color() const { return m_color; }

	void setText(std::string const& _text) { m_text = _text; update(); }
	void setFont(Font const& _font) { m_font = _font; update(); }
	void setColor(Color const& _color) { m_color = _color; update(); }

protected:
	MarkdownViewBody(std::string const& _text = "", Color _c = NullColor, Font const& _font = Font());

	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);

private:
	std::string m_text;
	Font m_font;
	Color m_color;
};

}


