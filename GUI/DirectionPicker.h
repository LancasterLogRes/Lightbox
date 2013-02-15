#pragma once

#include <Numeric/Ellipse.h>
#include "View.h"

namespace Lightbox
{

class DirectionPickerBody;
typedef boost::intrusive_ptr<DirectionPickerBody> DirectionPicker;

class DirectionPickerBody: public ViewCreator<ViewBody, DirectionPickerBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, DirectionPickerBody> Super;

	enum Mode { Point, Circle, Fill };

	~DirectionPickerBody();

	fEllipse direction() const { return m_direction; }
	Mode mode() { return m_mode; }

	void setDirection(fEllipse _e, bool _userEvent = false) { if (m_direction != _e) { m_direction = _e; directionChanged(_userEvent); } }
	void setPos(fCoord _e, bool _userEvent = false) { if (m_direction.pos() != _e) { m_direction.setPos(_e); directionChanged(_userEvent); } }
	void setRadii(fSize _e, bool _userEvent = false) { if (m_direction.radii() != _e) { m_direction.setRadii(_e); directionChanged(_userEvent); } }
	void setMode(Mode _m, bool _userEvent = false) { if (m_mode != _m) { m_mode = _m; directionChanged(_userEvent); } }
	void setOnDirectionChanged(EventHandler const& _t) { m_onDirectionChanged = _t; }
	void setColor(Color _c) { m_color = _c; update(); }
	void setMiddle(Color _c) { if (m_middle != _c) { m_middle = _c; /*update();*/ } }	// Not required for now.

	DirectionPicker withOnDirectionChanged(EventHandler const& _t) { setOnDirectionChanged(_t); return this; }

protected:
	explicit DirectionPickerBody(Color _c = White, Grouping _grouping = NoGrouping);
	
	virtual void initGraphics();
	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned _l);
	virtual void directionChanged(bool _userEvent) { if (m_onDirectionChanged && _userEvent) m_onDirectionChanged(this); update(2); }
	virtual fSize specifyFit(fSize _space) const;

private:
	float xC(float _sign) { return m_direction.x() + sin(Pi / 4.f) * _sign * m_direction.rx(); }
	float yC(float _sign) { return m_direction.y() + sin(Pi / 4.f) * _sign * m_direction.ry(); }
	float wC(float _xc) { return fabs(_xc - m_direction.x()) / sin(Pi / 4); }
	float hC(float _yc) { return fabs(_yc - m_direction.y()) / sin(Pi / 4); }

	fEllipse m_direction;
	Mode m_mode;
	Color m_color;
	Color m_middle;
	Grouping m_grouping;

	iSize m_lastSign;
	bool m_dragCenter;

	EventHandler m_onDirectionChanged;
};

}


