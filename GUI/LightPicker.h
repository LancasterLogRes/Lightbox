#pragma once

#include <Common/Color.h>
#include <LGL/Program.h>
#include "View.h"

namespace Lightbox
{

class LightPickerBody;
typedef boost::intrusive_ptr<LightPickerBody> LightPicker;

class LightPickerBody: public ViewCreator<ViewBody, LightPickerBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, LightPickerBody> Super;
	~LightPickerBody();

	float light() const { return m_light; }
	Color middle() const { return m_middle; }

	void setLight(float _l, bool _userEvent = false) { m_light = _l; lightChanged(_userEvent); }
	void setMiddle(Color _c) { if (_c.hue() != m_middle.hue()) { m_middle = _c; update(); } }
	void setOnLightChanged(EventHandler const& _t) { m_onLightChanged = _t; }

	LightPicker withOnLightChanged(EventHandler const& _t) { setOnLightChanged(_t); return this; }

protected:
	LightPickerBody();
	
	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);
	virtual fSize specifyFit(fSize _space) const;

	virtual void lightChanged(bool _userEvent) { if (m_onLightChanged && _userEvent) m_onLightChanged(this); update(1); }

	virtual void initGraphics();
	virtual void finiGraphics();
	
private:
	Program m_lightBar;
	EventHandler m_onLightChanged;
	float m_light;
	Color m_middle;
};

}


