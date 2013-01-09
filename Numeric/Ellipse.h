#pragma once

#include "Coord.h"

namespace Lightbox
{

template <typename Numeric>
class Ellipse: public Quad<Numeric>
{
	typedef Quad<Numeric> Super;
public:
	typedef Coord<Numeric> xCoord;
	typedef Size<Numeric> xSize;

	Ellipse() {}
	Ellipse(xCoord _centre, xSize _radii): Super(_centre.x(), _centre.y(), _radii.w(), _radii.h()) {}
	Ellipse(xCoord _centre, Numeric _radius): Super(_centre.x(), _centre.y(), _radius, _radius) {}
	explicit Ellipse(xSize _size): Super(0, 0, _size.w(), _size.h()) {}
	Ellipse(Numeric _x, Numeric _y, Numeric _rx, Numeric _ry): Super(_x, _y, _rx, _ry) {}
	Ellipse(Numeric _x, Numeric _y, Numeric _r): Super(_x, _y, _r, _r) {}
	template<class _N> explicit Ellipse(Ellipse<_N> _s): Super(_s.x(), _s.y(), _s.rx(), _s.ry()) {}

	bool operator==(Ellipse _c) const { return compare(_c); }
	bool operator!=(Ellipse _c) const { return !compare(_c); }

	Super& asQuad() { return (Super&)*this; }

	void translate(xSize _s) { sum(Super(_s.x(), _s.y(), 0, 0)); }
	void move(xCoord _pos) { Super::setX(_pos.x()); Super::setY(_pos.y()); }
	void setPos(xCoord _p) { move(_p); }
	void setRadii(xSize _r) { Super::setZ(_r.w()); Super::setW(_r.h()); }

	using Super::setX;
	using Super::setY;
	void setXRadius(Numeric _xr) { Super::setZ(_xr); }
	void setYRadius(Numeric _yr) { Super::setW(_yr); }

	using Super::x;
	using Super::y;

	xCoord pos() const { return xCoord(x(), y()); }
	xSize radii() const { return xSize(Super::z(), Super::w()); }
	Numeric rx() const { return Super::z(); }
	Numeric ry() const { return Super::w(); }
	Numeric left() const { return x() - Super::z(); }
	Numeric top() const { return y() - Super::w(); }
	Numeric right() const { return x() + Super::z(); }
	Numeric bottom() const { return y() + Super::w(); }
	Numeric l() const { return left(); }
	Numeric t() const { return top(); }
	Numeric r() const { return right(); }
	Numeric b() const { return bottom(); }
	xCoord topLeft() const { return xCoord(left(), top()); }
	xCoord topRight() const { return xCoord(right(), top()); }
	xCoord bottomLeft() const { return xCoord(left(), bottom()); }
	xCoord bottomRight() const { return xCoord(right(), bottom()); }
	xCoord middle() const { return fCoord(x(), y()); }

	Ellipse expanded(Numeric _f) const { return Ellipse(x(), y(), Super::z() + _f, Super::w() + _f); }
	Ellipse shrunk(Numeric _f) const { return Ellipse(x(), y(), Super::z() - _f, Super::w() - _f); }
	Ellipse inset(Numeric _x, Numeric _y) const { return Ellipse(x(), y(), Super::z() - _x, Super::w() - _y); }
	Ellipse translated(xSize _s) const { return Ellipse(x() + _s.w(), y() + _s.h(), Super::z(), Super::w()); }
	Ellipse multipliedBy(xSize _s) const { return Ellipse(x() * _s.w(), y() * _s.h(), Super::z() * _s.w(), Super::w() * _s.h()); }

	Ellipse operator+(Numeric _f) const { return expanded(_f); }
	Ellipse operator-(Numeric _f) const { return shrunk(_f); }
	Ellipse operator/(xSize _s) const { return dividedBy(_s); }
	Ellipse operator*(xSize _s) const { return multipliedBy(_s); }
};

typedef Ellipse<float> fEllipse;
typedef Ellipse<double> dEllipse;
typedef Ellipse<int> iEllipse;
typedef Ellipse<unsigned> uEllipse;

template<class _S, class _T> _S& operator<<(_S& _out, Ellipse<_T> const& _me)
{
	_out << "O" << _me.pos() << "+/-" << _me.radii();
	return _out;
}

}
