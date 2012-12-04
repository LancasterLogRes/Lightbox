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

	virtual fSize specifyMinimumSize() const;
	
protected:
	TextLabelBody(std::string _text = "");

	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);

private:
	std::string m_text;
	Font m_font;
	Color m_color;
};

}


