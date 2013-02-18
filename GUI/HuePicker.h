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

	void setHue(float _h, bool _userEvent = false) { if (!isFinite(m_hue) || m_hue != _h) { m_hue = _h; hueChanged(_userEvent); } }
	void setMiddle(Color _c) { if (m_middle != _c) { m_middle = _c; update(1); } }
	void setOnHueChanged(EventHandler const& _t) { m_onHueChanged = _t; assert(!!m_onHueChanged); }

	HuePicker withOnHueChanged(EventHandler const& _t) { setOnHueChanged(_t); return this; }

protected:
	HuePickerBody(Color _c = White, Grouping _grouping = NoGrouping);
	
	virtual bool event(Event* _e);
	virtual void preDraw(unsigned);
	virtual void draw(Slate const& _c, unsigned _l);
	virtual void initGraphics();
	virtual void finiGraphics();
	virtual void updateLayers();
	virtual void resized();

	virtual fSize specifyMinimumSize(fSize) const;
	virtual fSize specifyMaximumSize(fSize) const;
	virtual fSize specifyFit(fSize _space) const;

	virtual void hueChanged(bool _userEvent) { if (m_onHueChanged && _userEvent) m_onHueChanged(this); update(1); }

private:
	float m_hue;
	Color m_middle;
	Program m_hueWheel;
	Program m_hueWheelDodge;
	EventHandler m_onHueChanged;
	Texture2D m_hueWheelTex;
};

}


