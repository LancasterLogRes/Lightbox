#pragma once

#include "Vector.h"

#undef _N

namespace Lightbox
{

template<class Numeric> class Size;

template<class Numeric>
class Coord: public CalcPair<Numeric, Coord<Numeric> >
{
	typedef CalcPair<Numeric, Coord<Numeric> > Super;

public:
	Coord() {}
	Coord(Pair<Numeric> _q): Super(_q) {}
	Coord(Super _q): Super(_q) {}
	Coord(Numeric _x, Numeric _y): Super(_x, _y) {}
	template <class _N> explicit Coord(Coord<_N> _s): CalcPair<Numeric, Coord<Numeric>>(_s) {}

	using Super::x;
	using Super::y;
	
	bool operator<(Coord const& _c) const { return x() < _c.x() && y() < _c.y(); }
	bool operator>(Coord const& _c) const { return x() > _c.x() && y() > _c.y(); }
	bool operator<=(Coord const& _c) const { return x() <= _c.x() && y() <= _c.y(); }
	bool operator>=(Coord const& _c) const { return x() >= _c.x() && y() >= _c.y(); }
	bool operator==(Coord const& _c) const { return compare(_c); }
	bool operator!=(Coord const& _c) const { return !compare(_c); }

	Coord operator+(Size<Numeric> const& _c) const;
	Coord operator-(Size<Numeric> const& _c) const;
	Size<Numeric> operator-(Coord const& _c) const;

	Coord& operator+=(Size<Numeric> const& _c);
	Coord& operator-=(Size<Numeric> const& _c);
};

typedef Coord<float> XY;

typedef Coord<float> fCoord;
typedef Coord<double> dCoord;
typedef Coord<int> iCoord;
typedef Coord<unsigned> uCoord;

template<class Numeric>
class Size: public CalcPair<Numeric, Size<Numeric> >
{
	typedef CalcPair<Numeric, Size<Numeric> > Super;

public:
	Size() {}
	Size(Pair<Numeric> _q): Super(_q) {}
	Size(Super _q): Super(_q) {}
	Size(Numeric _w, Numeric _h): Super(_w, _h) {}
	explicit Size(Coord<Numeric> _c): Super((Super)_c) {}
	template<class _N> explicit Size(Size<_N> _s): CalcPair<Numeric, Size<Numeric>>(_s) {}

	Numeric w() const { return x(); }
	Numeric h() const { return y(); }
	Numeric width() const { return x(); }
	Numeric height() const { return y(); }
	Numeric area() const { return w() * h(); }
	Numeric perimeter() const { return (w() + h()) * 2; }

	void setW(Numeric _w) { Super::setX(_w); }
	void setH(Numeric _h) { Super::setY(_h); }
	void setWidth(Numeric _w) { Super::setX(_w); }
	void setHeight(Numeric _h) { Super::setY(_h); }

	bool operator<(Size const& _c) const { return w() < _c.w() && h() < _c.h(); }
	bool operator>(Size const& _c) const { return w() > _c.w() && h() > _c.h(); }
	bool operator<=(Size const& _c) const { return w() <= _c.w() && h() <= _c.h(); }
	bool operator>=(Size const& _c) const { return w() >= _c.w() && h() >= _c.h(); }
	bool operator==(Size const& _c) const { return compare(_c); }
	bool operator!=(Size const& _c) const { return !compare(_c); }

private:
	using Super::x;
	using Super::y;
	using Super::setX;
	using Super::setY;
};

typedef Size<float> fSize;
typedef Size<double> dSize;
typedef Size<int> iSize;
typedef Size<unsigned> uSize;

template <class Numeric> Coord<Numeric> Coord<Numeric>::operator+(Size<Numeric> const& _c) const { return Coord(x() + _c.w(), y() + _c.h()); }
template <class Numeric> Coord<Numeric> Coord<Numeric>::operator-(Size<Numeric> const& _c) const { return Coord(x() - _c.w(), y() - _c.h()); }
template <class Numeric> Size<Numeric> Coord<Numeric>::operator-(Coord const& _c) const { return Size<Numeric>(x() - _c.x(), y() - _c.y()); }
template <class Numeric> Coord<Numeric>& Coord<Numeric>::operator+=(Size<Numeric> const& _c) { setX(x() + _c.w()); setY(y() + _c.h()); return *this; }
template <class Numeric> Coord<Numeric>& Coord<Numeric>::operator-=(Size<Numeric> const& _c) { setX(x() - _c.w()); setY(y() - _c.h()); return *this; }

template<class _S, class _T> _S& operator<<(_S& _out, Coord<_T> const& _me)
{
	_out << "(" << _me.x() << "," << _me.y() << ")";
	return _out;
}

template<class _S, class _T> _S& operator<<(_S& _out, Size<_T> const& _me)
{
	_out << "(" << _me.w() << "x" << _me.h() << ")";
	return _out;
}

}
