#pragma once

#include <Numeric/Coord.h>

#include "Global.h"

namespace Lightbox
{

class Layout
{
	friend class ViewBody;
public:
	virtual void layout(fSize _s) = 0;
	virtual fSize minimumSize() = 0;
	virtual fSize maximumSize() = 0;

protected:
	ViewBody* m_view;	// Only allowed here as this will get deleted in the view's destructor.
};

class OverlayLayout: public Layout
{
public:
	OverlayLayout(fVector4 _margins): m_margins(_margins) {}
	virtual void layout(fSize _s);
	virtual fSize minimumSize();
	virtual fSize maximumSize();

private:
	fVector4 m_margins;
};

class HorizontalLayout: public Layout
{
public:
	virtual void layout(fSize _s);
	virtual fSize minimumSize();
	virtual fSize maximumSize();
};

class VerticalLayout: public Layout
{
public:
	virtual void layout(fSize _s);
	virtual fSize minimumSize();
	virtual fSize maximumSize();
};

}


