#pragma once

#include <functional>
#include <Common/Flags.h>
#include "Font.h"
#include "View.h"

namespace Lightbox
{

class BasicButtonBody;
typedef boost::intrusive_ptr<BasicButtonBody> BasicButton;

enum GroupingFlags { NoGrouping = 0, HorizontalGrouping = 1, VerticalGrouping = 2, ForceAbove = 4, ForceBelow = 8, ForceLeft = 16, ForceRight= 32 };
typedef Flags<GroupingFlags> Grouping;

LIGHTBOX_FLAGS_TYPE(GroupingFlags, Grouping);

class BasicButtonBody: public ViewCreator<ViewBody, BasicButtonBody>
{
	friend class ViewBody;

public:
	virtual ~BasicButtonBody() {}

	static Layers layers();

	bool isDown() const { return m_isDown; }
	bool isLit() const { return m_isLit; }
	std::string const& text() const { return m_text; }
	Color color() const { return m_color; }
	Grouping grouping() const { return m_grouping; }

	void setOnTapped(EventHandler const& _t) { m_onTapped = _t; }
	void setText(std::string const& _s) { m_text = _s; update(); }
	void setColor(Color _c) { m_color = _c; update(); }
	void setGrouping(Grouping _g) { m_grouping = _g; update(); }

	void setDown(bool _down = true);
	void setLit(bool _lit = true);

	BasicButton withOnTapped(EventHandler const& _t) { setOnTapped(_t); return this; }
	BasicButton withText(std::string const& _s) { setText(_s); return this; }
	BasicButton withColor(Color _c) { setColor(_c); return this; }
	BasicButton withGrouping(Grouping _g) { setGrouping(_g); return this; }

protected:
	BasicButtonBody(std::string const& _text = std::string(), Color _c = White, Font _f = Font(), Grouping _grouping = NoGrouping);
	BasicButtonBody(std::string const& _text, Color _c, Grouping _grouping): BasicButtonBody(_text, _c, Font(), _grouping) {}

	void drawButton(Context const& _c, unsigned _l, bool _down, std::function<void(iRect)> const& _inner = std::function<void(iRect)>(), bool _polish = true);

	virtual void draw(Context const& _c, unsigned _layer);
	virtual bool event(Event* _e);
	virtual void tapped();
	virtual void pushed() { setDown(); setLit(); }
	virtual void released(bool _withFinger) { setDown(false); setLit(false); if (_withFinger) tapped(); }
	virtual fSize specifyMinimumSize(fSize) const;
	virtual void initGraphics();
	virtual void enabledChanged();
	virtual void updateLayers() { layer(1).show(isEnabled() && isLit()); }

private:
	std::string m_text;
	EventHandler m_onTapped;
	Color m_color;
	Font m_font;
	Grouping m_grouping;

	bool m_isLit;
	bool m_isDown;
	int m_downPointer;
};

template <class _T> inline _T const& operator<<(_T const& _t, BasicButton const& _a) { _a->setOnTapped(_t); return _t; }

}

