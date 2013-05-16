#pragma once

#include <Common/Color.h>
#include "Font.h"
#include "View.h"

namespace lb
{

class TextLabelBody;
typedef boost::intrusive_ptr<TextLabelBody> TextLabel;

enum AlignmentFlags
{
	AlignCenter = 0,
	AlignTop = 1,
	AlignBottom = 2,
	AlignBaselineCenter = 4,
	AlignLeft = 16,
	AlignRight = 32,
	AlignTightVertical = 256
};

LIGHTBOX_FLAGS_TYPE(AlignmentFlags, Alignment);

class TextLabelBody: public ViewCreator<ViewBody, TextLabelBody>
{
	friend class ViewBody;
	
public:
	virtual ~TextLabelBody();

	std::string const& text() const { return m_text; }
	Font const& font() const { return m_font; }
	Color const& color() const { return m_color; }
	Alignment alignment() const { return m_alignment; }

	void setText(std::string const& _text) { ViewLock l(this); m_text = _text; update(); }
	void setFont(Font const& _font) { ViewLock l(this); m_font = _font; update(); }
	void setColor(Color const& _color) { m_color = _color; update(); }
	void setRule(bool _enable = true) { m_rule = _enable; update(); }
	void setAlignment(Alignment _a) { m_alignment = _a; update(); }

	TextLabel withRule(bool _enable = true) { setRule(_enable); return this; }

protected:
	TextLabelBody(std::string const& _text = "", Color _c = NullColor, Font const& _font = Font(), Alignment _a = AlignCenter);

	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);
	virtual fSize specifyMinimumSize(fSize) const;

private:
	std::string m_text;
	Font m_font;
	Color m_color;
	bool m_rule;
	Alignment m_alignment;
};

}


