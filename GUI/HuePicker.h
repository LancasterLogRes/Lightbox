#pragma once

#include <LGL/Program.h>
#include "ToggleButton.h"

namespace Lightbox
{

class HuePickerBody;
typedef boost::intrusive_ptr<HuePickerBody> HuePicker;

class HuePickerBody: public ViewCreator<ToggleButtonBody, HuePickerBody>
{
	friend class ViewBody;
	
public:
	virtual ~HuePickerBody();

	float hue() const { return m_hue; }

	void setHue(float _h, bool _userEvent = false) { if (m_hue != _h) { m_hue = _h; hueChanged(_userEvent); } }
	void setOnHueChanged(EventHandler const& _t) { m_onHueChanged = _t; }

	HuePicker withOnHueChanged(EventHandler const& _t) { setOnHueChanged(_t); return this; }

protected:
	HuePickerBody(Color _c = White, Grouping _grouping = NoGrouping);
	
	virtual bool event(Event* _e);
	virtual void draw(Context const& _c, unsigned _l);
	virtual void initGraphics();
	virtual void finiGraphics();

	virtual fSize specifyMinimumSize(fSize) const;
	virtual fSize specifyMaximumSize(fSize) const;
	virtual fSize specifyFit(fSize _space) const;

	virtual void hueChanged(bool _userEvent) { if (m_onHueChanged && _userEvent) m_onHueChanged(this); update(); }

private:
	float m_hue;
	Program m_hueWheel;
	EventHandler m_onHueChanged;
};

}


