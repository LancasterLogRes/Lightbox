#pragma once

#include "View.h"

namespace Lightbox
{

class BasicButtonBody;
typedef boost::intrusive_ptr<BasicButtonBody> BasicButton;

class BasicButtonBody: public ViewCreator<ViewBody, BasicButtonBody>
{
	friend class ViewBody;

public:
	virtual ~BasicButtonBody() {}
	
	std::string const& text() const { return m_text; }
	BasicButton setText(std::string const& _s) { m_text = _s; update(); return this; }
	template <class _T> BasicButton setOnTapped(_T const& _t) { m_onTapped = _t; return this; }
	std::function<void(BasicButton)> const& onTapped() const { return m_onTapped; }

	virtual void draw(Context const& _c);
	virtual bool event(Event* _e);

	virtual fSize specifyMinimumSize(fSize) const;

protected:
	BasicButtonBody(std::string const& _text = std::string());

	virtual void tapped();

	std::string m_text;
	std::function<void(BasicButton)> m_onTapped;	// TODO: move to BasicButton; retrieve from parent.

	bool m_isDown;
	int m_downPointer;
};

template <class _T> inline _T const& operator<<(_T const& _t, BasicButton const& _a) { _a->setOnTapped(_t); return _t; }

}

