#pragma once

#include "View.h"

namespace Lightbox
{

class FrameBody;
typedef boost::intrusive_ptr<FrameBody> Frame;

class FrameBody: public ViewCreator<ViewBody, FrameBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, FrameBody> Super;
	~FrameBody();
	
protected:
	FrameBody();
	
	virtual bool event(Event* _e);
	
private:
};

View operator|(View const& _a, View const& _b);

}


