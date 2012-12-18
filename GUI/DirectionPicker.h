#pragma once

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

	fCoord direction() const { return m_direction; }
	fSize radius() const { return m_radius; }
	Mode mode() { return m_mode; }

	void setDirection(fCoord _d, bool _userEvent = false) { if (m_direction != _d) { m_direction = _d; changed(_userEvent); } }
	void setRadius(fSize _s, bool _userEvent = false) { if (m_radius != _s) { m_radius = _s; changed(_userEvent); } }
	void setMode(Mode _m, bool _userEvent = false) { if (m_mode != _m) { m_mode = _m; changed(_userEvent); } }
	void setOnChanged(EventHandler const& _t) { m_onChanged = _t; }

	DirectionPicker withOnChanged(EventHandler const& _t) { setOnChanged(_t); return this; }

protected:
	DirectionPickerBody();
	
	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);
	virtual void changed(bool _userEvent) { if (m_onChanged && _userEvent) m_onChanged(this); update(); }
	virtual fSize specifyFit(fSize _space) const;

private:
	float xC(float _sign) { return m_direction.x() + sin(Pi / 4.f) * _sign * m_radius.w() / 2; }
	float yC(float _sign) { return m_direction.y() + sin(Pi / 4.f) * _sign * m_radius.h() / 2; }
	float wC(float _xc) { return fabs(_xc - m_direction.x()) * 2 / sin(Pi / 4); }
	float hC(float _yc) { return fabs(_yc - m_direction.y()) * 2 / sin(Pi / 4); }

	fCoord m_direction;
	fSize m_radius;
	Mode m_mode;

	iSize m_lastSign;
	bool m_dragCenter;

	EventHandler m_onChanged;
};

}


