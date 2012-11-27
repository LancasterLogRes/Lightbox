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
	Rect(xCoord _min, xSize _size): m_pos(_min), m_size(_size) {}
	Rect(xCoord _min, xCoord _max): m_pos(_min), m_size(_max - _min) {}
	explicit Rect(xSize _size): m_pos(0, 0), m_size(_size) {}
	Rect(Numeric _x, Numeric _y, Numeric _w, Numeric _h): m_pos(_x, _y), m_size(_w, _h) {}
	template<class _N> explicit Rect(Rect<_N> _s): m_pos(_s.pos()), m_size(_s.size()) {}
	
	Vector4<Numeric>& asVector4() { return (Vector4<Numeric>&)*this; }

	void move(xCoord _pos) { m_pos = _pos; }
	void resize(xSize _s) { m_size = _s; }
	void alterTopLeft(xCoord _pos) { m_size += m_pos - _pos; m_pos = _pos; }
	void alterBottomRight(xCoord _max) { m_size = _max - m_pos; }

	void setX(Numeric _x) { m_pos.setX(_x); }
	void setY(Numeric _y) { m_pos.setY(_y); }
	void setW(Numeric _w) { m_size.setW(_w); }
	void setH(Numeric _h) { m_size.setH(_h); }
	void setWidth(Numeric _w) { m_size.setW(_w); }
	void setHeight(Numeric _h) { m_size.setH(_h); }
	
	xCoord pos() const { return m_pos; }
	xSize size() const { return m_size; }
	Numeric x() const { return m_pos.x(); }
	Numeric y() const { return m_pos.y(); }
	Numeric w() const { return m_size.w(); }
	Numeric h() const { return m_size.h(); }
	Numeric width() const { return m_size.w(); }
	Numeric height() const { return m_size.h(); }
	Numeric left() const { return m_pos.x(); }
	Numeric top() const { return m_pos.y(); }
	Numeric right() const { return m_pos.x() + m_size.w(); }
	Numeric bottom() const { return m_pos.y() + m_size.h(); }
	Numeric l() const { return m_pos.x(); }
	Numeric t() const { return m_pos.y(); }
	Numeric r() const { return m_pos.x() + m_size.w(); }
	Numeric b() const { return m_pos.y() + m_size.h(); }
	xCoord topLeft() const { return m_pos; }
	xCoord topRight() const { return xCoord(right(), m_pos.y()); }
	xCoord bottomLeft() const { return xCoord(m_pos.x(), bottom()); }
	xCoord bottomRight() const { return xCoord(right(), bottom()); }
	xCoord middle() const { return m_pos + m_size / 2; }

	xCoord lerp(float _x, float _y) const { return m_pos + m_size * xCoord(_x, _y); }

	Rect expanded(Numeric _f) const { return Rect(m_pos - _f, m_size + 2 * _f); }
	Rect shrunk(Numeric _f) const { return Rect(m_pos + _f, m_size - 2 * _f); }
	
	Rect dividedBy(xSize _s) const { return Rect(m_pos / _s, m_size / _s); }

	Rect operator+(Numeric _f) const { return expanded(_f); }
	Rect operator-(Numeric _f) const { return shrunk(_f); }
	Rect operator/(xSize _s) const { return dividedBy(_s); }
	bool contains(xCoord const& _p) const { return _p >= m_pos && _p <= bottomRight(); }
	// TODO
	bool inside(Rect const&) const { return true; }

private:
	xCoord m_pos;
	xSize m_size;
};

typedef Rect<float> fRect;
typedef Rect<double> dRect;
typedef Rect<int> iRect;
typedef Rect<unsigned> uRect;

}
