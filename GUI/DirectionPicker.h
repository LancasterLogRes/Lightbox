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

	void setDirection(fCoord _d) { if (m_direction != _d) { m_direction = _d; changed(); } }
	void setRadius(fSize _s) { if (m_radius != _s) { m_radius = _s; changed(); } }
	fCoord direction() const { return m_direction; }
	fSize radius() const { return m_radius; }

	void setMode(Mode _m) { if (m_mode != _m) { m_mode = _m; changed(); } }
	Mode mode() { return m_mode; }

	template <class _T> DirectionPicker setOnChanged(_T const& _t) { m_onChanged = _t; return this; }
	std::function<void(DirectionPicker const&)> const& onChanged() const { return m_onChanged; }
	
protected:
	DirectionPickerBody();
	
	virtual bool event(Event* _e);
	virtual bool draw(Context const& _c);

	virtual void changed() { if (m_onChanged) m_onChanged(this); update(); }

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

	std::function<void(DirectionPicker const&)> m_onChanged;
};

}


