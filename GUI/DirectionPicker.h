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

	void setDirection(fCoord _d) { if (m_direction != _d) { m_direction = _d; onChanged(); } }
	void setRadius(fSize _s) { if (m_radius != _s) { m_radius = _s; onChanged(); } }
	fCoord direction() const { return m_direction; }
	fSize radius() const { return m_radius; }

	void setMode(Mode _m) { if (m_mode != _m) { m_mode = _m; onChanged(); } }
	Mode mode() { return m_mode; }

	template <class _T> DirectionPicker setOnChanged(_T const& _t) { m_onChanged = _t; return this; }
	std::function<void(DirectionPicker const&)> const& onChanged() const { return m_onChanged; }
	
protected:
	DirectionPickerBody();
	
	virtual bool event(Event* _e);
	virtual bool draw(Context const& _c);

	virtual void changed() { if (m_onChanged) m_onChanged(this); update(); }

private:
	fCoord m_direction;
	fSize m_radius;
	Mode m_mode;

	std::function<void(DirectionPicker const&)> m_onChanged;
};

}


