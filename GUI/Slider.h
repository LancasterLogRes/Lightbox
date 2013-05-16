#pragma once

#include <Common/Color.h>
#include <LGL/Program.h>
#include "View.h"

namespace lb
{

class SliderBody;
typedef boost::intrusive_ptr<SliderBody> Slider;

class SliderBody: public ViewCreator<ViewBody, SliderBody>
{
	friend class ViewBody;

public:
	typedef ViewCreator<ViewBody, SliderBody> Super;
	~SliderBody();

	float value() const { return m_value; }

	void setValue(float _l, bool _userEvent = false) { m_value = clamp(_l, m_min, m_max); changed(_userEvent); }
	void setBounds(float _min, float _max) { if (_min != m_min || _max != m_max) { m_min = _min; m_max = _max; m_value = clamp(m_value, m_min, m_max); update(1); } }
	void setOnChanged(EventHandler const& _t) { m_onChanged = _t; }

	Slider withOnChanged(EventHandler const& _t) { setOnChanged(_t); return this; }

protected:
	SliderBody(Orientation _o = Horizontal, float _value = 1.f, float _min = 0.f, float _max = 1.f);

	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);
	virtual fSize specifyFit(fSize _space) const;

	virtual void changed(bool _userEvent) { if (m_onChanged && _userEvent) m_onChanged(this); update(1); }

	virtual void initGraphics();
	virtual void finiGraphics();

private:
	EventHandler m_onChanged;
	Orientation m_o;
	float m_min;
	float m_max;
	float m_value;
};

}


