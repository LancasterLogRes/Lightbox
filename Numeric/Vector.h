#pragma once

#include <Common/Global.h>

#define cross %
#define dot *

#undef _S
#undef _T

namespace Lightbox
{

template<class T> class Pair;

template<class _S, class _T> _S& operator>>(_S& _in, Pair<_T>& _me);
template<class _S, class _T> _S& operator<<(_S& _out, Pair<_T> const& _me);

static const double s_tolerance = 1e-8;

template<class T>
class Pair
{
	typedef Pair<T> This;

public:
	Pair() {}
	Pair(T _x, T _y) : m_x(_x), m_y(_y) {}
	Pair(T const* _data) : m_x(_data[0]), m_y(_data[1]) {}
	template<typename _T> explicit Pair(_T const* _data) : m_x(_data[0]), m_y(_data[1]) {}
	template<typename _T> explicit Pair(Pair<_T> _data) : m_x(_data.x()), m_y(_data.y()) {}

	This& asPair() { return *this; }
	This const& asPair() const { return *this; }

	This& sum(T _n) { m_x += _n; m_y += _n; return *this; }
	This& sub(T _n) { m_x -= _n; m_y -= _n; return *this; }
	This& scale(T _n) { m_x *= _n; m_y *= _n; return *this; }
	This& slash(T _n) { m_x /= _n; m_y /= _n; return *this; }

	This& sum(This _n) { m_x += _n.m_x; m_y += _n.m_y; return *this; }
	This& sub(This _n) { m_x -= _n.m_x; m_y -= _n.m_y; return *this; }
	This& scale(This _n) { m_x *= _n.m_x; m_y *= _n.m_y; return *this; }
	This& slash(This _n) { m_x /= _n.m_x; m_y /= _n.m_y; return *this; }

	This summed(T _n) const { return This(m_x + _n, m_y + _n); }
	This subbed(T _n) const { return This(m_x - _n, m_y - _n); }
	This scaled(T _n) const { return This(m_x * _n, m_y * _n); }
	This slashed(T _n) const { return This(m_x / _n, m_y / _n); }

	This summed(This _n) const { return This(m_x + _n.m_x, m_y + _n.m_y); }
	This subbed(This _n) const { return This(m_x - _n.m_x, m_y - _n.m_y); }
	This scaled(This _n) const { return This(m_x * _n.m_x, m_y * _n.m_y); }
	This slashed(This _n) const { return This(m_x / _n.m_x, m_y / _n.m_y); }

	void setX(T _x) { m_x = _x; }
	void setY(T _y) { m_y = _y; }
	template<typename _T> void set(_T _x, _T _y) { m_x = _x; m_y = _y; }

	T x() const { return m_x; }
	T y() const { return m_y; }
	T const* data() const { return &m_x; }

	bool compare(This _c) const { return m_x == _c.m_x && m_y == _c.m_y; }

	T length() const { return sqrt(length()); }
	T lengthSquared() const { return x()*x() + y()*y(); }

protected:
	T m_x;
	T m_y;
};

template<class T, typename R>
class CalcPair: public Pair<T>
{
public:
	typedef Pair<T> Super;

	CalcPair() {}
	CalcPair(T _x, T _y): Pair<T>(_x, _y) {}
	CalcPair(T const* _data): Pair<T>(_data) {}
	template<typename _T> explicit CalcPair(_T const* _data): Pair<T>(_data) {}
	template<typename _T> explicit CalcPair(Pair<_T> const& _data): Pair<T>(_data) {}

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

template<class T>
class Vector2: public CalcPair<T, Vector2<T> >
{
	typedef CalcPair<T, Vector2<T> > Super;

public:
	Vector2()
#if DEBUG
	: Super(69.69, 69.69)
#endif
	{}

	Vector2<T>(Pair<T> const& _q): Super(_q) {}
	Vector2<T>(T _x, T _y): Super(_x, _y) {}
	template<class _T> explicit Vector2<T>(Vector2<_T> _n): Super(_n) {}

	using Super::x;
	using Super::y;
	using Super::setX;
	using Super::setY;
	using Super::length;
	using Super::lengthSquared;

	bool operator==(Vector2<T> _c) const { return compare(_c); }
	bool operator!=(Vector2<T> _c) const { return !compare(_c); }

	T const* asArray2() const { return Super::data(); }
	T* asArray2() { return Super::data(); }

	T dotted(Vector2<T> _b) const { return x() * _b.x() + y() * _b.y(); }

	void normalize() { T l = length(); if (l > 0) { setX(x() / l); setY(y() / l); } }
	Vector2<T> normalized() const { Vector2<T> ret(*this); ret.normalize(); return ret; }

	bool isSignificant() const { return lengthSquared() > s_tolerance; }
	bool approximates(Vector2<T> const& _cmp) const { return ((*this) - _cmp).lengthSquared() < s_tolerance; }
};

typedef Vector2<unsigned> uVector2;
typedef Vector2<int> iVector2;
typedef Vector2<double> dVector2;
typedef Vector2<float> fVector2;

typedef Vector2<double> dvec2;
typedef Vector2<float> vec2;
typedef Vector2<unsigned> uvec2;
typedef Vector2<int> ivec2;
typedef Vector2<double> dV2;
typedef Vector2<float> V2;
typedef Vector2<unsigned> uV2;
typedef Vector2<int> iV2;

template <class _T> inline Vector2<_T> operator*(_T _s, Vector2<_T> _v) { return _v *= _s; }
template <class _T> inline Vector2<_T> operator/(_T _s, Vector2<_T> _v) { return _v /= _s; }

template <class T> class Vector3;
template <class _S, class _T> _S& operator<<(_S& _out, Vector3<_T> const& _me);
template <class _S, class _T> _S& operator>>(_S& _in, Vector3<_T>& _me);

template<class T>
class Vector3: public CalcQuad<T, Vector3<T> >
{
	typedef CalcQuad<T, Vector3<T> > Super;

public:
	Vector3<T>()
#if DEBUG
	: Super(69.69, 69.69, 69.69)
#endif
	{}

	Vector3<T>(Quad<T> _q): Super(_q) {}
	Vector3<T>(Super _q): Super(_q) {}
	Vector3<T>(T _x, T _y, T _z): Super(_x, _y, _z) {}
	Vector3<T>(T const _d[3]): Super(_d) {}
	template<class _T> explicit Vector3<T>(_T const _d[3]): Super(_d) {}
	template<class _T> explicit Vector3<T>(Vector3<_T> _n): Super(_n) {}

	using Super::x;
	using Super::y;
	using Super::z;
	using Super::setX;
	using Super::setY;
	using Super::setZ;

	using Super::operator*;
	T operator*(Vector3<T> _b) const { return dotted(_b); }
	Vector3<T> operator%(Vector3<T> _b) const { return crossed(_b); }
	Vector3<T>& operator^=(Vector3<T> _b) { return crossWith(_b); }

	bool operator==(Vector3<T> _b) const { return compare3(_b); }
	bool operator!=(Vector3<T> _b) const { return !compare3(_b); }

	T length() const { return Super::length3(); }
	T lengthSquared() const { return Super::length3Squared(); }
	static Vector3<T> aggregate(Vector3<T> _p0, T _w0, Vector3<T> _p1, T _w1) { return Super::aggregate(_p0, _w0, _p1, _w1); }
	static Vector3<T> aggregate(Vector3<T> _p0, T _w0, Vector3<T> _p1, T _w1, Vector3<T> _p2, T _w2) { return Super::aggregate(_p0, _w0, _p1, _w1, _p2, _w2); }
	static Vector3<T> average(Vector3<T> _p0, Vector3<T> _p1) { return Super::aggregate(_p0, _p1); }
	static Vector3<T> average(Vector3<T> _p0, Vector3<T> _p1, Vector3<T> _p2) { return Super::aggregate(_p0, _p1, _p2); }

	T dotted(Vector3<T> _b) const { return x() * _b.x() + y() * _b.y() + z() * _b.z(); }
	Vector3<T> crossed(Vector3<T> _b) const { return Vector3<T>(y() * _b.z() - z() * _b.y(), z() * _b.x() - x() * _b.z(), x() * _b.y() - y() * _b.x()); }
	Vector3<T>& crossWith(Vector3<T> _b) { Quad<T>::set(y() * _b.z() - z() * _b.y(), z() * _b.x() - x() * _b.z(), x() * _b.y() - y() * _b.x()); return *this; }

	void normalize() { normalise(); }
	Vector3<T> normalized() const { return normalised(); }
	Vector3<T> normalised() const { return Super::slashed(length()); }
	void normalise() { Super::slash(length()); }
	void normaliseTo(T _mag) { slash(length() / _mag); }
	
//	const fVector2 toPolar() const { return fVector2(frac((abs(x()) > 0.0 ? atan2(z(), x()) : (z() > 0.0 ? PIBY2 : -PIBY2)) / PI2 + 1.0),  acos(y() / length()) / PI); }
	Vector2<T> toPolar() const { return Vector2<T>(frac(atan2(z(), x()) / TwoPi + 1.0), acos(y() / length()) / Pi); }

	bool isSignificant() const { return lengthSquared() > s_tolerance; }
	bool isXYSignificant() const { return sqr(x()) + sqr(y()) > s_tolerance; }
	bool isParallelTo(Vector3<T> const& _v) const { return !crossed(_v).isSignificant(); }
	bool isPerpendicularTo(Vector3<T> const& _v) const { return fabs(dotted(_v)) < s_tolerance; }
	bool approximates(Vector3<T> const& _cmp) const { return ((*this) - _cmp).lengthSquared() < s_tolerance; }

	// KILL
	T const* asArray3() const { return Super::data(); }
	T* asArray3() { return Super::data(); }

private:
	using Super::w;
	using Super::setW;
	using Super::compare;
};

template <class _T> inline Vector3<_T> operator*(_T _s, Vector3<_T> _v) { return _v *= _s; }
template <class _T> inline Vector3<_T> operator/(_T _s, Vector3<_T> _v) { return _v /= _s; }

typedef Vector3<double> dVector3;
typedef Vector3<float> fVector3;
typedef Vector3<unsigned> uVector3;
typedef Vector3<int> iVector3;

typedef Vector3<double> dvec3;
typedef Vector3<float> vec3;
typedef Vector3<unsigned> uvec3;
typedef Vector3<int> ivec3;
typedef Vector3<double> dV3;
typedef Vector3<float> V3;
typedef Vector3<unsigned> uV3;
typedef Vector3<int> iV3;

static const fVector3 Xaxis = fVector3(1, 0, 0);
static const fVector3 Yaxis = fVector3(0, 1, 0);
static const fVector3 Zaxis = fVector3(0, 0, 1);

template<class T = double>
class Vector4: public CalcQuad<T, Vector4<T> >
{
	typedef CalcQuad<T, Vector4<T> > Super;

public:
	Vector4<T>()
#if DEBUG
	: Super(69.69, 69.69, 69.69, 69.69)
#endif
	{}

	Vector4<T>(Quad<T> _q): Super(_q) {}
	Vector4<T>(Super _q): Super(_q) {}
	Vector4<T>(T _x, T _y, T _z, T _w): Super(_x, _y, _z, _w) {}
	Vector4<T>(T const _d[4]): Super(_d) {}
	template<class _T> explicit Vector4<T>(_T const _d[4]): Super(_d) {}
	template<class _T> explicit Vector4<T>(Vector4<_T> _n): Super(_n) {}

	using Super::x;
	using Super::y;
	using Super::z;
	using Super::w;

	bool operator==(Vector4<T> _c) const { return compare(_c); }
	bool operator!=(Vector4<T> _c) const { return !compare(_c); }

	static Vector4<T> aggregate(Vector4<T> _p0, T _w0, Vector4<T> _p1, T _w1) { return Super::aggregate(_p0, _w0, _p1, _w1); }
	static Vector4<T> aggregate(Vector4<T> _p0, T _w0, Vector4<T> _p1, T _w1, Vector4<T> _p2, T _w2) { return Super::aggregate(_p0, _w0, _p1, _w1, _p2, _w2); }
	static Vector4<T> average(Vector4<T> _p0, Vector4<T> _p1) { return Super::aggregate(_p0, _p1); }
	static Vector4<T> average(Vector4<T> _p0, Vector4<T> _p1, Vector4<T> _p2) { return Super::aggregate(_p0, _p1, _p2); }

	T dotted(Vector3<T> _b) const { return x() * _b.x() + y() * _b.y() + z() * _b.z() + w() * _b.w(); }

private:
	using Super::length3;
	using Super::compare3;
};

typedef Vector4<double> dVector4;
typedef Vector4<float> fVector4;
typedef Vector4<unsigned> uVector4;
typedef Vector4<int> iVector4;

typedef Vector4<double> dvec4;
typedef Vector4<float> vec4;
typedef Vector4<unsigned> uvec4;
typedef Vector4<int> ivec4;
typedef Vector4<double> dV4;
typedef Vector4<float> V4;
typedef Vector4<unsigned> uV4;
typedef Vector4<int> iV4;


template<class _S, class _T> _S& operator>>(_S& _in, Pair<_T>& _me)
{
	while (_in && _in.get() != '(') {}
	_in >> _me.m_x;
	if (_in.get() == ',')
		_in >> _me.m_y;
	_in.get();
	return _in;
}

template<class _S, class _T> _S& operator<<(_S& _out, Pair<_T> const& _me)
{
	_out << "(" << _me.x() << ", " << _me.y() << ")";
	return _out;
}

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

template<class _S, class _T> _S& operator>>(_S& _in, Vector3<_T>& _me)
{
	while (_in && _in.get() != '(') {}
	_T t = 0;
	_in >> t;
	_me.setX(t);
	if (_in.get() == ',')
		_in >> t;
	_me.setY(t);
	if (_in.get() == ',')
		_in >> t;
	_me.setZ(t);
	_in.get();
	return _in;
}

template<class _S, class _T> _S& operator<<(_S& _out, Vector3<_T> const& _me)
{
	_out << "(" << _me.x() << ", " << _me.y() << ", " << _me.z() << ")";
	return _out;
}

inline std::string toString(fVector3 const& _v)
{
	std::stringstream ss;
	ss << "fVector3(" << _v.x() << ", " << _v.y() << ", " << _v.z() << ")";
	return ss.str();
}


}
