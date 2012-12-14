#pragma once

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
	void setMiddle(Color _c) { m_middle = _c; update(); }
	void setOnLightChanged(EventHandler const& _t) { m_onLightChanged = _t; }

	LightPicker withOnLightChanged(EventHandler const& _t) { setOnLightChanged(_t); return this; }

protected:
	LightPickerBody();
	
	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);
	virtual fSize specifyFit(fSize _space) const;

	void lightChanged(bool _userEvent) { if (m_onLightChanged && _userEvent) m_onLightChanged(this); update(); }
	
private:
	Program m_lightBar;
	EventHandler m_onLightChanged;
	float m_light;
	Color m_middle;
};

}


