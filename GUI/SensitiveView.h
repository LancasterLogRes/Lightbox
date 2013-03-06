#pragma once

#include "View.h"

namespace Lightbox
{

class SensitiveViewBody;
typedef boost::intrusive_ptr<SensitiveViewBody> SensitiveView;

class SensitiveViewBody: public ViewCreator<ViewBody, SensitiveViewBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, SensitiveViewBody> Super;
	~SensitiveViewBody();

	void setOnTapped(EventHandler const& _h) { m_onTapped = _h; }
	SensitiveView withOnTapped(EventHandler const& _h) { setOnTapped(_h); return this; }

protected:
	SensitiveViewBody();
	
	virtual bool pushed(unsigned, fCoord) { return true; }
	virtual void released(unsigned, bool _withFinger, fCoord) { if (_withFinger) m_onTapped(this); }
	virtual void draw(Slate const& _c, unsigned _l);
	
private:
	EventHandler m_onTapped;
};

}


