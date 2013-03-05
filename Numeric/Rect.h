#pragma once

#include "Coord.h"
#include "Margin.h"

namespace Lightbox
{

template<typename Numeric>
class Rect
{
public:
	typedef Coord<Numeric> xCoord;
	typedef Size<Numeric> xSize;
	typedef Margin<Numeric> xMargin;

	Rect() {}
	explicit Rect(xSize _size): m_pos(0, 0), m_size(_size) {}
	Rect(xCoord _min, xSize _size): m_pos(_min), m_size(_size) {}
	Rect(xCoord _min, xCoord _max): m_pos(_min), m_size(_max - _min) {}
	Rect(Numeric _x, Numeric _y, Numeric _w, Numeric _h): m_pos(_x, _y), m_size(_w, _h) {}
	template <class _N> explicit Rect(Rect<_N> _s): m_pos(_s.pos()), m_size(xCoord(_s.bottomRight()) - m_pos) {}

	static Rect square(xCoord _mid, Numeric _halfWidth) { return Rect(_mid - fCoord(_halfWidth, _halfWidth), fSize(_halfWidth, _halfWidth) * 2); }
	
	Vector4<Numeric>& asVector4() { return (Vector4<Numeric>&)*this; }

	void translate(xSize _s) { m_pos += _s; }
	Rect translated(xSize _s) const { return Rect(m_pos + _s, m_size); }
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

	xCoord lerp(float _x, float _y) const { return m_pos + xSize(m_size.w() * _x, m_size.h() * _y); }
	Rect lerp(float _xf, float _yf, float _xt, float _yt) const { xCoord o = lerp(_xf, _yf); return Rect(o, lerp(_xt, _yt) - o); }
	xCoord lerp(fSize _xy) const { return m_pos + xSize(m_size.w() * _xy.w(), m_size.h() * _xy.h()); }
	Rect lerpAlign(xSize _s, float _x, float _y) const { return Rect(xCoord(Size<float>(size() - _s) * Size<float>(_x, _y)) + pos(), _s); }

	Rect flippedHorizontal() const { return Rect(x() + w(), y(), -w(), h()); }
	Rect flippedVertical() const { return Rect(x(), y() + h(), w(), -h()); }
	Rect flipped() const { return Rect(pos() + size(), -size()); }

	Rect inset(Numeric _s) const { return inset(xMargin(_s, _s, _s, _s)); }
	Rect inset(Numeric _x, Numeric _y) const { return inset(xSize(_x, _y)); }
	Rect inset(xSize _s) const { return Rect(m_pos + _s, m_size - 2 * _s); }
	Rect inset(xSize _tl, xSize _br) const { return inset(xMargin(_tl, _br)); }
	Rect inset(Numeric _l, Numeric _t, Numeric _r, Numeric _b) const { return inset(xMargin(_l, _t, _r, _b)); }
	Rect inset(xMargin _m) const { return Rect(m_pos.x() + _m.left(), m_pos.y() + _m.top(), m_size.w() - _m.left() - _m.right(), m_size.h() - _m.top() - _m.bottom()); }

	Rect outset(Numeric _s) const { return outset(xMargin(_s, _s, _s, _s)); }
	Rect outset(Numeric _x, Numeric _y) const { return outset(xSize(_x, _y)); }
	Rect outset(xSize _s) const { return Rect(m_pos - _s, m_size + 2 * _s); }
	Rect outset(xSize _tl, xSize _br) const { return outset(xMargin(_tl, _br)); }
	Rect outset(Numeric _l, Numeric _t, Numeric _r, Numeric _b) const { return outset(xMargin(_l, _t, _r, _b)); }
	Rect outset(xMargin _m) const { return Rect(m_pos.x() - _m.left(), m_pos.y() - _m.top(), m_size.w() + _m.left() + _m.right(), m_size.h() + _m.top() + _m.bottom()); }

	Rect multipliedBy(Numeric _s) const { return Rect(m_pos * _s, m_size * _s); }
	Rect multipliedBy(xSize _s) const { return Rect(m_pos * _s, m_size * _s); }
	Rect dividedBy(Numeric _s) const { return Rect(m_pos / _s, m_size / _s); }
	Rect dividedBy(xSize _s) const { return Rect(m_pos / _s, m_size / _s); }

	Rect operator+(Numeric _f) const { return outset(_f); }
	Rect operator-(Numeric _f) const { return inset(_f); }
	Rect operator*(Numeric _s) const { return multipliedBy(_s); }
	Rect operator*(xSize _s) const { return multipliedBy(_s); }
	Rect operator/(Numeric _s) const { return dividedBy(_s); }
	Rect operator/(xSize _s) const { return dividedBy(_s); }
	bool contains(xCoord _p) const { return _p >= m_pos && _p <= bottomRight(); }
	void include(xCoord _p) { Coord<Numeric> br = bottomRight().max(_p); Coord<Numeric> tl = m_pos.minify(_p); m_size = br - tl; }
	// TODO
	bool inside(Rect const&) const { return true; }
	bool isAbove(Rect const& _r) const { return _r.top() >= bottom(); }
	bool isBelow(Rect const& _r) const { return _r.bottom() <= top(); }

	bool operator==(Rect const& _c) const { return m_pos == _c.m_pos && m_size == _c.m_size; }
	bool operator!=(Rect const& _c) const { return !operator==(_c); }
	bool operator<(Rect const& _c) const { return inside(_c); }

private:
	xCoord m_pos;
	xSize m_size;
};

typedef Rect<float> fRect;
typedef Rect<double> dRect;
typedef Rect<int> iRect;
typedef Rect<unsigned> uRect;

template<class _S, class _T> _S& operator<<(_S& _out, Rect<_T> const& _me)
{
	_out << "@" << _me.pos() << "+" << _me.size();
	return _out;
}

}
