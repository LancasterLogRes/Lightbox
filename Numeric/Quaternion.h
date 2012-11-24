#pragma once

#include "Vector.h"

namespace Lightbox
{

template<class T = double>
class Quaternion : public CalcQuad<T, Quaternion<T> >
{
	typedef Quaternion<T> This;
	typedef CalcQuad<T, Quaternion<T> > Super;

public:
	Quaternion() {}
	Quaternion(Quad<T> _q): Super(_q) {}
	Quaternion(T _x, T _y, T _z, T _w): Super(_x, _y, _z, _w) {}
	template<class _T> Quaternion<T>(Vector3<_T> _v, _T _w): Super(_v.x(), _v.y(), _v.z(), _w) {}

	Super::x;
	Super::y;
	Super::z;
	Super::w;
	Super::setX;
	Super::setY;
	Super::setZ;
	Super::setW;

	Super::operator-;
	This operator-() const { return conjugate(); }
	Super::operator*;
	This operator*(This _b) const
	{	return This(
			w()*_b.x() + x()*_b.w() + y()*_b.z() - z()*_b.y(),
			w()*_b.y() - x()*_b.z() + y()*_b.w() + z()*_b.x(),
			w()*_b.z() + x()*_b.y() - y()*_b.x() + z()*_b.w(),
			w()*_b.w() - x()*_b.x() - y()*_b.y() - z()*_b.z());
	}
	Super::operator*=;
	This operator*=(This _b) { set(w()*_b.x() + x()*_b.w() + y()*_b.z() - z()*_b.y(), w()*_b.y() - x()*_b.z() + y()*_b.w() + z()*_b.x(), w()*_b.z() + x()*_b.y() - y()*_b.x() + z()*_b.w(), w()*_b.w() - x()*_b.x() - y()*_b.y() - z()*_b.z()); return *this; }

	bool operator==(This _c) const { return compare(_c); }
	bool operator!=(This _c) const { return !compare(_c); }

	static const This rotation(T _x, T _y, T _z, T _theta) { return This(_x * sin(_theta/2.0), _y * sin(_theta/2.0), _z * sin(_theta/2.0), cos(_theta/2.0)); }
	template<class V> static This rotation(Vector3<V> _v, T _theta)
	{	return This(
			_v.x() * sin(_theta / 2.0),
			_v.y() * sin(_theta / 2.0),
			_v.z() * sin(_theta / 2.0),
			cos(_theta / 2.0));
	}

	Vector3<T> discardW() const { return Vector3<T>(Super::asQuad()); }

	Super::length;
	Super::length3;

	This normalised() const { return This(slashed(length())); }
	This conjugate() const { return This(scaled(This(-1, -1, -1, 1))); }
	void normalise() { slash(length()); }
	void conjuggle() { scale(This(-1, -1, -1, 1)); }

	/**
	 * Quaternions can be used as a compound vector stored as a unit length vector and a length.
	 * This allows them to be used as a unit length without giving up the ability to adjust
	 * individual components. To do this, just use this method.
	 */
	static Quaternion compound(Vector3<T> _sum) { return Quaternion(_sum.normalised(), _sum.length()); }
	void changeComponent(Vector3<T> _old, Vector3<T> _cur) { scale(w()); sub(_old); sum(_cur); auto l3 = Super::length3(); set3(x() / l3, y() / l3, z() / l3, l3); }

	template <class _T> friend _T& operator<<(_T& _out, This const& _me) { return _out << "[" << _me.x() << ", " << _me.y() << ", " << _me.z() << ";" << _me.w() << "]"; }
};

}
