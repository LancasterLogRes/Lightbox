#pragma once

#include <Common/Color.h>
#include "Font.h"
#include "View.h"

namespace Lightbox
{

class TextLabelBody;
typedef boost::intrusive_ptr<TextLabelBody> TextLabel;

class TextLabelBody: public ViewCreator<ViewBody, TextLabelBody>
{
	friend class ViewBody;
	
public:
	virtual ~TextLabelBody();

	std::string const& text() const { return m_text; }
	Font const& font() const { return m_font; }
	Color const& color() const { return m_color; }

	void setText(std::string const& _text) { m_text = _text; update(); }
	void setFont(Font const& _font) { m_font = _font; update(); }
	void setColor(Color const& _color) { m_color = _color; update(); }

protected:
	TextLabelBody(std::string const& _text = "", Color _c = NullColor, Font const& _font = Font());

	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);
	virtual fSize specifyMinimumSize(fSize) const;

private:
	std::string m_text;
	Font m_font;
	Color m_color;
};

}


