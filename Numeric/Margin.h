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
	explicit Margin(xSize _xy): Super(_xy.w(), _xy.h(), _xy.w(), _xy.h()) {}
	Margin(xSize _tl, xSize _br): Super(_tl.w(), _tl.h(), _br.w(), _br.h()) {}
	Margin(Numeric _left, Numeric _top, Numeric _right, Numeric _bottom): Super(_left, _top, _right, _bottom) {}

	bool operator==(Margin _c) const { return compare(_c); }
	bool operator!=(Margin _c) const { return !compare(_c); }

	Numeric left() const { return Super::m_x; }
	Numeric top() const { return Super::m_y; }
	Numeric right() const { return Super::m_z; }
	Numeric bottom() const { return Super::m_w; }

	void setLeft(Numeric _v) { Super::m_x = _v; }
	void setTop(Numeric _v) { Super::m_y = _v; }
	void setRight(Numeric _v) { Super::m_z = _v; }
	void setBottom(Numeric _v) { Super::m_w = _v; }

	Margin withLeft(Numeric _v) const { return Margin(_v, m_y, m_z, m_w); }
	Margin withTop(Numeric _v) const { return Margin(m_x, _v, m_z, m_w); }
	Margin withRight(Numeric _v) const { return Margin(m_x, m_y, _v, m_w); }
	Margin withBottom(Numeric _v) const { return Margin(m_x, m_y, m_z, _v); }

	xSize extra() const { return xSize(m_x + m_z, m_y + m_w); }
	Numeric extraWidth() const { return m_x + m_z; }
	Numeric extraHeight() const { return m_y + m_w; }

private:
	using Super::compare;
	using Super::m_x;
	using Super::m_y;
	using Super::m_z;
	using Super::m_w;
};

typedef Margin<int> iMargin;
typedef Margin<unsigned> uMargin;
typedef Margin<float> fMargin;
typedef Margin<double> dMargin;

}


