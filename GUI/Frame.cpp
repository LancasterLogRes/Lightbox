#include <Common/Global.h>
#include "Global.h"
#include "Frame.h"
using namespace std;
using namespace Lightbox;

FrameBody::FrameBody()
{
	setLayers(Layers());
}

FrameBody::~FrameBody()
{
}

bool FrameBody::event(Event* _e)
{
	return Super::event(_e);
}

View Lightbox::operator|(View const& _a, View const& _b)
{
	if (!!_a->children().size() == !!_b->children().size())
	{
		View ret = FrameBody::create();
		_a->setParent(ret);
		_b->setParent(ret);
		return ret;
	}
	else if (_a->children().size())
	{
		_b->setParent(_a);
		return _a;
	}
	else
	{
		_a->setParent(_b);
		return _b;
	}
}

