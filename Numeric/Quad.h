#pragma once

#include <Common/Global.h>

#undef _S
#undef _T

namespace Lightbox
{

template<class T> class Quad;

template<class _S, class _T> _S& operator<<(_S& _out, Quad<_T> const& _me);
template<class _S, class _T> _S& operator>>(_S& _in, Quad<_T>& _me);

// TODO: reimplement using SIMD
template<class T>
class Quad
{
	typedef Quad<T> This;

public:
	Quad() {}
	Quad(T _x, T _y, T _z, T _w = 1.0) : m_x(_x), m_y(_y), m_z(_z), m_w(_w) {}
	Quad(T const* _data) : m_x(_data[0]), m_y(_data[1]), m_z(_data[2]), m_w(_data[3]) {}
	template<typename _T> explicit Quad(_T const* _data) : m_x(_data[0]), m_y(_data[1]), m_z(_data[2]), m_w(_data[3]) {}
	template<typename _T> explicit Quad(Quad<_T> _data) : m_x(_data.x()), m_y(_data.y()), m_z(_data.z()), m_w(_data.w()) {}

	Quad<T>& asQuad() { return *this; }
	Quad<T> const& asQuad() const { return *this; }

	static This aggregate(This const& p0, T w0, This const& p1, T w1) { return This(p0.x() * w0 + p1.x() * w1, p0.y() * w0 + p1.y() * w1, p0.z() * w0 + p1.z() * w1, p0.w() * w0 + p1.w() * w1); }
	static This aggregate(This const& p0, T w0, This const& p1, T w1, This const& p2, T w2) { return This(p0.x() * w0 + p1.x() * w1 + p2.x() * w2, p0.y() * w0 + p1.y() * w1 + p2.y() * w2, p0.z() * w0 + p1.z() * w1 + p2.z() * w2, p0.w() * w0 + p1.w() * w1 + p2.w() * w2); }
	static This average(This const& p0, This const& p1) { return This((p0.x() + p1.x()) / 2.0, (p0.y() + p1.y()) / 2.0, (p0.z() + p1.z()) / 2.0, (p0.w() + p1.w()) / 2.0); }
	static This average(This const& p0, This const& p1, This const& p2) { return This((p0.x() + p1.x() + p2.x()) / 3.0, (p0.y() + p1.y() + p2.y()) / 3.0, (p0.z() + p1.z() + p2.z()) / 3.0, (p0.w() + p1.w() + p2.w()) / 3.0); }

	void sum(T _n) { m_x += _n; m_y += _n; m_z += _n; m_w += _n; }
	void sub(T _n) { m_x -= _n; m_y -= _n; m_z -= _n; m_w -= _n; }
	void scale(T _n) { m_x *= _n; m_y *= _n; m_z *= _n; m_w *= _n; }
	void slash(T _n) { m_x /= _n; m_y /= _n; m_z /= _n; m_w /= _n; }
	void sum(This _n) { m_x += _n.m_x; m_y += _n.m_y; m_z += _n.m_z; m_w += _n.m_w; }
	void sub(This _n) { m_x -= _n.m_x; m_y -= _n.m_y; m_z -= _n.m_z; m_w -= _n.m_w; }
	void scale(This _n) { m_x *= _n.m_x; m_y *= _n.m_y; m_z *= _n.m_z; m_w *= _n.m_w; }
	void slash(This _n) { m_x /= _n.m_x; m_y /= _n.m_y; m_z /= _n.m_z; m_w /= _n.m_w; }

	This summed(T _n) const { return This(m_x + _n, m_y + _n, m_z + _n, m_w + _n); }
	This subbed(T _n) const { return This(m_x - _n, m_y - _n, m_z - _n, m_w - _n); }
	This scaled(T _n) const { return This(m_x * _n, m_y * _n, m_z * _n, m_w * _n); }
	This slashed(T _n) const { return This(m_x / _n, m_y / _n, m_z / _n, m_w / _n); }
	This summed(This _n) const { return This(m_x + _n.m_x, m_y + _n.m_y, m_z + _n.m_z, m_w + _n.m_w); }
	This subbed(This _n) const { return This(m_x - _n.m_x, m_y - _n.m_y, m_z - _n.m_z, m_w - _n.m_w); }
	This scaled(This _n) const { return This(m_x * _n.m_x, m_y * _n.m_y, m_z * _n.m_z, m_w * _n.m_w); }
	This slashed(This _n) const { return This(m_x / _n.m_x, m_y / _n.m_y, m_z / _n.m_z, m_w / _n.m_w); }

	template<typename _T> void setX(_T _x) { m_x = _x; }
	template<typename _T> void setY(_T _y) { m_y = _y; }
	template<typename _T> void setZ(_T _z) { m_z = _z; }
	template<typename _T> void setW(_T _w) { m_w = _w; }
	template<typename _T> void set(_T _x, _T _y, _T _z) { m_x = _x; m_y = _y; m_z = _z; }
	template<typename _T> void set(_T _x, _T _y, _T _z, _T _w) { m_x = _x; m_y = _y; m_z = _z; m_w = _w; }
	template<typename _T> void set(_T const* _data) { m_x = _data[0]; m_y = _data[1]; m_z = _data[2]; m_w = _data[3]; }
	template<typename _T> void set3(_T const* _data) { m_x = _data[0]; m_y = _data[1]; m_z = _data[2]; }

	T x() const { return m_x; }
	T y() const { return m_y; }
	T z() const { return m_z; }
	T w() const { return m_w; }
	T* data() { return &m_x; }
	T const* data() const { return &m_x; }

	T length() const { return sqrt(x() * x() + y() * y() + z() * z() + w() * w()); }
	T length3() const { return sqrt(x() * x() + y() * y() + z() * z()); }
	T lengthSquared() const { return x() * x() + y() * y() + z() * z() + w() * w(); }
	T length3Squared() const { return x() * x() + y() * y() + z() * z(); }

	bool compare(This _c) const { return m_x == _c.m_x && m_y == _c.m_y && m_z == _c.m_z && m_w == _c.m_w; }
	bool compare3(This _c) const { return m_x == _c.m_x && m_y == _c.m_y && m_z == _c.m_z; }

protected:
	T m_x;
	T m_y;
	T m_z;
	T m_w;
};

template<class T, typename R>
class CalcQuad: public Quad<T>
{
public:
	typedef Quad<T> Super;

	CalcQuad() {}
	CalcQuad(T _x, T _y, T _z, T _w = 1.0): Quad<T>(_x, _y, _z, _w) {}
	CalcQuad(T const* _data): Quad<T>(_data) {}
	template<typename _T> explicit CalcQuad(_T const* _data): Quad<T>(_data) {}
	template<typename _T> explicit CalcQuad(Quad<_T> const& _data): Quad<T>(_data) {}

	using Super::summed;
	using Super::subbed;
	using Super::scaled;
	using Super::slashed;
	using Super::sum;
	using Super::scale;
	using Super::sub;
	using Super::slash;

	R operator+(R _x) const { return R(summed(_x)); }
	R operator-(R _x) const { return R(subbed(_x)); }
	R operator*(R _x) const { return R(scaled(_x)); }
	R operator/(R _x) const { return R(slashed(_x)); }
	R operator+(T _x) const { return R(summed(_x)); }
	R operator-(T _x) const { return R(subbed(_x)); }
	R operator*(T _x) const { return R(scaled(_x)); }
	R operator/(T _x) const { return R(slashed(_x)); }

	R operator-() const { return R(scaled(T(-1))); }

	inline friend R operator*(T _a, R _b) { return R(_b.scaled(_a)); }
	inline friend R operator/(T _a, R _b) { return R(_b.slashed(_a)); }

	R& operator+=(R _x) { sum(_x); return *(R*)this; }
	R& operator-=(R _x) { sub(_x); return *(R*)this; }
	R& operator*=(R _x) { scale(_x); return *(R*)this; }
	R& operator/=(R _x) { slash(_x); return *(R*)this; }
	R& operator+=(T _x) { sum(_x); return *(R*)this; }
	R& operator-=(T _x) { sub(_x); return *(R*)this; }
	R& operator*=(T _x) { scale(_x); return *(R*)this; }
	R& operator/=(T _x) { slash(_x); return *(R*)this; }
};

template<class _S, class _T> _S& operator>>(_S& _in, Quad<_T>& _me)
{
	while (_in && _in.get() != '(') {}
	_in >> _me.m_x;
	if (_in.get() == ',')
		_in >> _me.m_y;
	if (_in.get() == ',')
		_in >> _me.m_z;
	if (_in.get() == ',')
		_in >> _me.m_w;
	_in.get();
	return _in;
}

template<class _S, class _T> _S& operator<<(_S& _out, Quad<_T> const& _me)
{
	_out << "(" << _me.x() << ", " << _me.y() << ", " << _me.z() << ", " << _me.w() << ")";
	return _out;
}

}
