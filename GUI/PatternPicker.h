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
	uSize space() const { return m_space; }

	void setSpace(uSize _s) { m_space = _s; update(); }
	void setIndex(int _i, bool _userEvent = false) { if (m_index != _i) { m_index = _i; indexChanged(_userEvent); } }
	void setOnIndexChanged(EventHandler const& _t) { m_onIndexChanged = _t; }

	PatternPicker withOnIndexChanged(EventHandler const& _t) { setOnIndexChanged(_t); return this; }

protected:
	PatternPickerBody(Color _c = White, Grouping _grouping = NoGrouping);

	virtual void indexChanged(bool _userEvent) { if (m_onIndexChanged && _userEvent) m_onIndexChanged(this); update(); }
	
	virtual bool event(Event* _e);
	virtual void draw(Context const& _c);
	
private:
	int m_index;
	uSize m_space;
	EventHandler m_onIndexChanged;
};

}


