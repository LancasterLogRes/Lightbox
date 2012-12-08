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
	typedef ViewCreator<ViewBody, TextLabelBody> Super;
	
public:
	virtual ~TextLabelBody();

	virtual fSize specifyMinimumSize(fSize) const;
	
protected:
	TextLabelBody(std::string const& _text = "", Color _c = NullColor, Font const& _font = Font());

	virtual bool event(Event* _e);
	virtual bool draw(Context const& _c);

private:
	std::string m_text;
	Font m_font;
	Color m_color;
};

}


