#pragma once

#include <Common/Global.h>
#include <Common/UnitTesting.h>
#include <Common/Maths.h>
#include "Pair.h"
#include "Quad.h"

#define cross %
#define dot *

#undef _S
#undef _T

namespace Lightbox
{

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

template <class _T> struct ApproximationTest<Vector2<_T>> { static bool test(Vector2<_T> _a, Vector2<_T> _b) { return _a.approximates(_b); } };

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

	Vector3<T> normalized() const { return Super::slashed(length()); }
	void normalize() { Super::slash(length()); }
	void normalizeTo(T _mag) { slash(length() / _mag); }
	
//	const fVector2 toPolar() const { return fVector2(frac((abs(x()) > 0.0 ? atan2(z(), x()) : (z() > 0.0 ? PIBY2 : -PIBY2)) / PI2 + 1.0),  acos(y() / length()) / PI); }
	Vector2<T> toPolar() const { return Vector2<T>(frac(atan2(z(), x()) / twoPi<float>() + 1.0), acos(y() / length()) / pi<float>()); }

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

template <class _T> struct ApproximationTest<Vector3<_T>> { static bool test(Vector3<_T> _a, Vector3<_T> _b) { return _a.approximates(_b); } };

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

LIGHTBOX_UNITTEST(1, "fVector3")
{
	require(!Xaxis.approximates(-Xaxis), "approximates");
	require((Xaxis + Yaxis).approximates(fVector3(1, 1, 0)), "+");
	require((Xaxis - Yaxis).approximates(fVector3(1, -1, 0)), "-");
	require((Xaxis * 5).approximates(fVector3(5, 0, 0)), "*");
	require((Xaxis / 5).approximates(fVector3(0.2, 0, 0)), "/");
	fVector3 v = Zaxis;
	v += Xaxis;
	require(v.approximates(Zaxis + Xaxis), "+=");
	v -= Xaxis;
	require(v.approximates(Zaxis), "-=");
	v *= 5;
	require(v.approximates(Zaxis * 5), "*=");
	v /= 5;
	require(v.approximates(Zaxis), "/=");
	require(Xaxis.isPerpendicularTo(Yaxis),"isPerpendicularTo");
	require(Xaxis.isParallelTo(fVector3(-1,0,0)),"isParallelTo");
}

}
