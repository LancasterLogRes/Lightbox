#pragma once

#include "ToggleButton.h"

namespace Lightbox
{

class PatternPickerBody;
typedef boost::intrusive_ptr<PatternPickerBody> PatternPicker;

class PatternPickerBody: public ViewCreator<ToggleButtonBody, PatternPickerBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ToggleButtonBody, PatternPickerBody> Super;
	~PatternPickerBody();

	int index() const { return m_index; }
	void setIndex(int _i) { if (m_index != _i) { m_index = _i; indexChanged(); } }
	template <class _T> PatternPicker setOnIndexChanged(_T const& _t) { m_onIndexChanged = _t; return this; }
	std::function<void(PatternPicker const&)> const& onIndexChanged() const { return m_onIndexChanged; }

	uSize space() const { return m_space; }
	void setSpace(uSize _s) { m_space = _s; update(); }

protected:
	PatternPickerBody();

	virtual void indexChanged() { if (m_onIndexChanged) m_onIndexChanged(this); update(); }
	
	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);
	
private:
	int m_index;
	uSize m_space;
	std::function<void(PatternPicker const&)> m_onIndexChanged;
};

}


