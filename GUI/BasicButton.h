#pragma once

#include "View.h"

namespace Lightbox
{

class BasicButtonBody;
typedef std::shared_ptr<BasicButtonBody> BasicButton;

class BasicButtonBody: public ViewBody
{
	friend class ViewBody;

public:
	template <class ... _T> static BasicButton create(View const& _parent, _T ... _args) { return ViewBody::doCreate<BasicButtonBody, _T ...>(_parent, _args ...); }
	virtual ~BasicButtonBody() {}
	
	std::string const& text() const { return m_text; }
	void setText(std::string const& _s) { m_text = _s; update(); }
	template <class _T> void setOnTap(_T const& _t) { m_onTap = _t; }

	virtual void draw(Context _c);
	virtual bool event(Event* _e);

protected:
	BasicButtonBody(std::string const& _text): m_text(_text), m_isDown(false) {}

	std::string m_text;
	std::function<void(BasicButtonBody*)> m_onTap;	// TODO: move to BasicButton; retrieve from parent.

	bool m_isDown;
	int m_downPointer;
};

}

