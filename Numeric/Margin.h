#pragma once

#include "Quad.h"
#include "Coord.h"

namespace Lightbox
{

template <class Numeric>
class Margin: public CalcQuad<Numeric, Margin<Numeric> >
{
	typedef CalcQuad<Numeric, Margin<Numeric> > Super;

public:
	typedef Size<Numeric> xSize;

	Margin(): Super(0, 0, 0, 0) {}
	Margin(xSize _tl, xSize _br): Super(_tl.w(), _tl.h(), _br.w(), _br.h()) {}
	Margin(Numeric _left, Numeric _top, Numeric _right, Numeric _bottom): Super(_left, _top, _right, _bottom) {}

	Numeric left() const { return Super::m_x; }
	Numeric top() const { return Super::m_y; }
	Numeric right() const { return Super::m_z; }
	Numeric bottom() const { return Super::m_w; }

	void setLeft(Numeric _v) { Super::m_x = _v; }
	void setTop(Numeric _v) { Super::m_y = _v; }
	void setRight(Numeric _v) { Super::m_z = _v; }
	void setBottom(Numeric _v) { Super::m_w = _v; }
};

typedef Margin<int> iMargin;
typedef Margin<unsigned> uMargin;
typedef Margin<float> fMargin;
typedef Margin<double> dMargin;

}


