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

	enum Grouping { NoGrouping, Horizontal, Vertical };
	
	bool isDown() const { return m_isDown; }
	std::string const& text() const { return m_text; }
	Color color() const { return m_color; }
	Grouping grouping() const { return m_grouping; }

	void setOnTapped(EventHandler const& _t) { m_onTapped = _t; }
	void setText(std::string const& _s) { m_text = _s; update(); }
	void setColor(Color _c) { m_color = _c; update(); }
	void setGrouping(Grouping _g) { m_grouping = _g; update(); }

	BasicButton withOnTapped(EventHandler const& _t) { setOnTapped(_t); return this; }
	BasicButton withText(std::string const& _s) { setText(_s); return this; }
	BasicButton withColor(Color _c) { setColor(_c); return this; }
	BasicButton withGrouping(Grouping _g) { setGrouping(_g); return this; }

protected:
	BasicButtonBody(std::string const& _text = std::string(), Color _c = White, Grouping _grouping = NoGrouping);

	void drawButton(Context const& _c, unsigned _l, bool _lit, bool _down);

	virtual vector<iMargin> prepareDraw();
	virtual void draw(Context const& _c, unsigned _layer);
	virtual bool event(Event* _e);
	virtual void tapped();
	virtual fSize specifyMinimumSize(fSize) const;

private:
	std::string m_text;
	EventHandler m_onTapped;
	Color m_color;
	Grouping m_grouping;

	bool m_isDown;
	int m_downPointer;
};

template <class _T> inline _T const& operator<<(_T const& _t, BasicButton const& _a) { _a->setOnTapped(_t); return _t; }

}

