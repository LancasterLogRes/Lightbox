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
	
	bool isDown() const { return m_isDown; }
	std::string const& text() const { return m_text; }

	void setOnTapped(EventHandler const& _t) { m_onTapped = _t; }
	void setText(std::string const& _s) { m_text = _s; update(); }

	BasicButton withOnTapped(EventHandler const& _t) { setOnTapped(_t); return this; }
	BasicButton withText(std::string const& _s) { setText(_s); return this; }

protected:
	BasicButtonBody(std::string const& _text = std::string());

	virtual void draw(Context const& _c);
	virtual bool event(Event* _e);
	virtual void tapped();
	virtual fSize specifyMinimumSize(fSize) const;

private:
	std::string m_text;
	EventHandler m_onTapped;

	bool m_isDown;
	int m_downPointer;
};

template <class _T> inline _T const& operator<<(_T const& _t, BasicButton const& _a) { _a->setOnTapped(_t); return _t; }

}

