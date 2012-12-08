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
	void setLight(float _l) { m_light = _l; lightChanged(); }
	Color middle() const { return m_middle; }
	void setMiddle(Color _c) { m_middle = _c; update(); }

	template <class _T> LightPicker setOnLightChanged(_T const& _t) { m_onLightChanged = _t; return this; }
	std::function<void(LightPicker const&)> const& onLightChanged() const { return m_onLightChanged; }

protected:
	LightPickerBody();
	
	virtual bool event(Event* _e);
	virtual bool draw(Context const& _c);
	virtual fSize specifyFit(fSize _space) const;

	void lightChanged() { if (m_onLightChanged) m_onLightChanged(this); update(); }
	
private:
	Program m_lightBar;
	std::function<void(LightPicker const&)> m_onLightChanged;
	float m_light;
	Color m_middle;
};

}


