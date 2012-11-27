#pragma once

#include "View.h"

namespace Lightbox
{

class BasicButtonBody;
typedef std::shared_ptr<BasicButtonBody> BasicButton;

class BasicButtonBody: public ViewBody
{
public:
	template <class ... _T> static BasicButton create(_T ... _args) { return BasicButton(new BasicButtonBody(_args ...)); }
	virtual ~BasicButtonBody() {}
	
	virtual void draw(Context _c);

protected:
	BasicButtonBody(std::string const& _text): m_text(_text) {}
	BasicButtonBody(View& _parent, std::string const& _text = ""): ViewBody(_parent), m_text(_text) {}

	std::string m_text;
};

}

