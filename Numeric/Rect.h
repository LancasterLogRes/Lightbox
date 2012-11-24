#pragma once

#include "Coord.h"

namespace Lightbox
{

template<typename Numeric>
class Rect
{
public:
	typedef Coord<Numeric> xCoord;
	typedef Size<Numeric> xSize;

	Rect() {}
	Rect(xCoord _min, xCoord _max): m_min(_min), m_max(_max) {}
	explicit Rect(xSize _size): m_min(0, 0), m_max(_size.w(), _size.h()) {}
	Rect(Numeric _minx, Numeric _miny, Numeric _maxx, Numeric _maxy):
		m_min(xCoord(_minx, _miny)), m_max(xCoord(_maxx, _maxy)) {}
	template<class _N> explicit Rect(Rect<_N> _s): m_min(_s.min()), m_max(_s.max()) {}
	
	xCoord min() const { return m_min; }
	xCoord max() const { return m_max; }
	
	void setMin(xCoord _min) { m_min = _min; }
	void setMax(xCoord _max) { m_max = _max; }

	Numeric x() const { return m_min.x(); }
	Numeric y() const { return m_min.y(); }
	Numeric w() const { return m_max.x() - m_min.x(); }
	Numeric h() const { return m_max.y() - m_min.y(); }
	Numeric width() const { return m_max.x() - m_min.x(); }
	Numeric height() const { return m_max.y() - m_min.y(); }
	
	void setX(Numeric _x) { m_min.setX(_x); }
	void setY(Numeric _y) { m_min.setY(_y); }
	void setW(Numeric _w) { m_max.setX(m_min.x() + _w); }
	void setH(Numeric _h) { m_max.setY(m_min.y() + _h); }
	void setWidth(Numeric _w) { m_max.setX(m_min.x() + _w); }
	void setHeight(Numeric _h) { m_max.setY(m_min.y() + _h); }
	
	xSize size() const { return m_max - m_min; }
	void setSize(xSize _s) { m_max = m_min + _s; }
	
	Numeric left() const { return m_min.x(); }
	Numeric top() const { return m_min.y(); }
	Numeric right() const { return m_max.x(); }
	Numeric bottom() const { return m_max.y(); }
	Numeric l() const { return m_min.x(); }
	Numeric t() const { return m_min.y(); }
	Numeric r() const { return m_max.x(); }
	Numeric b() const { return m_max.y(); }
	
	xCoord topLeft() const { return m_min; }
	xCoord topRight() const { return xCoord(m_max.x(), m_min.y()); }
	xCoord bottomLeft() const { return xCoord(m_min.x(), m_max.y()); }
	xCoord bottomRight() const { return m_max; }
	
	Rect expanded(Numeric _f) const { return Rect(m_min - _f, m_max + _f); }
	Rect shrunk(Numeric _f) const { return Rect(m_min + _f, m_max - _f); }
	
	Rect operator+(Numeric _f) const { return Rect(m_min - _f, m_max + _f); }
	Rect operator-(Numeric _f) const { return Rect(m_min + _f, m_max - _f); }
	// TODO
	bool inside(Rect const&) const { return true; }
	bool contains(xCoord const& _p) const { return _p >= m_min && _p <= m_max; }
	
private:
	xCoord m_min;
	xCoord m_max;
};

typedef Rect<float> fRect;
typedef Rect<double> dRect;
typedef Rect<int> iRect;
typedef Rect<unsigned> uRect;

}
