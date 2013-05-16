#pragma once

#include <Numeric/Coord.h>
#include <Numeric/Margin.h>
#include "Global.h"

namespace lb
{

class Layout
{
	friend class ViewBody;
public:
	virtual ~Layout() {}

	virtual void layout(fSize _s) = 0;
	virtual fSize minimumSize(fSize) = 0;
	virtual fSize maximumSize(fSize) = 0;
	virtual fSize fit(fSize _space) = 0;

protected:
	ViewBody* m_view;	// Only allowed here as this will get deleted in the view's destructor.
};

class OverlayLayout: public Layout
{
public:
	OverlayLayout(fMargin _margin = fMargin()): m_margin(_margin) {}
	virtual void layout(fSize _s);
	virtual fSize minimumSize(fSize);
	virtual fSize maximumSize(fSize);
	virtual fSize fit(fSize _space);

private:
	fMargin m_margin;
};

class HorizontalLayout: public Layout
{
public:
	HorizontalLayout(float _spacing = 0.f, fMargin _margin = fMargin()): m_spacing(_spacing), m_margin(_margin) {}
	virtual void layout(fSize _s);
	virtual fSize minimumSize(fSize);
	virtual fSize maximumSize(fSize);
	virtual fSize fit(fSize _space);

private:
	fSize dead() const;

	float m_spacing;
	fMargin m_margin;
};

class VerticalLayout: public Layout
{
public:
	VerticalLayout(float _spacing = 0.f, fMargin _margin = fMargin()): m_spacing(_spacing), m_margin(_margin) {}
	virtual void layout(fSize _s);
	virtual fSize minimumSize(fSize);
	virtual fSize maximumSize(fSize);
	virtual fSize fit(fSize _space);

private:
	fSize dead() const;

	float m_spacing;
	fMargin m_margin;
};

}


