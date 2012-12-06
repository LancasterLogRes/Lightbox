#pragma once

#include "View.h"

namespace Lightbox
{

class HuePickerBody;
typedef boost::intrusive_ptr<HuePickerBody> HuePicker;

class HuePickerBody: public ViewCreator<ViewBody, HuePickerBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, HuePickerBody> Super;
	~HuePickerBody();

	float hue() const { return m_hue; }
	void setHue(float _h) { if (m_hue != _h) { m_hue = _h; update(); hueChanged(); } }
	template <class _T> HuePicker setOnHueChanged(_T const& _t) { m_onHueChanged = _t; return this; }
	std::function<void(HuePicker const&)> const& onHueChanged() const { return m_onHueChanged; }

protected:
	HuePickerBody();
	
	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);
	
	virtual fSize specifyMinimumSize() const;
	virtual fSize specifyMaximumSize() const;

	virtual void hueChanged() { if (m_onHueChanged) m_onHueChanged(this); }

private:
	float m_hue;
	Program m_hueWheel;
	std::function<void(HuePicker const&)> m_onHueChanged;
};

}


