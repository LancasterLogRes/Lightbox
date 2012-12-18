#pragma once

#include <string>
#include <sstream>
#include <cmath>
#include <Common/Maths.h>
#include <Common/UnitTesting.h>
#include "Vector.h"

namespace Lightbox
{

/** Quaternion class for rotations
 *
 * Global quaternion inquiry functions
 * Quaternion is
 * x = sin(theta) * v_x
 * y = sin(theta) * v_y
 * z = sin(theta) * v_z
 * w = cos(theta)
 */
template<class T = double>
class Quaternion: public CalcQuad<T, Quaternion<T> >
{
	typedef Quaternion<T> This;
	typedef CalcQuad<T, Quaternion<T> > Super;

public:
	Quaternion(): Super(1, 0, 0, 0) {}
	Quaternion(Quad<T> _q): Super(_q) {}
	Quaternion(T _x, T _y, T _z, T _w): Super(_x, _y, _z, _w) {}
	template<class _T> explicit Quaternion<T>(Vector3<_T> _v, _T _w = 0): Super(_v.x(), _v.y(), _v.z(), _w) {}
	Quaternion(T _a, Vector3<T> const& _bcd): Super(_bcd.x(), _bcd.y(), _bcd.z(), _a) {}

	using Super::x;
	using Super::y;
	using Super::z;
	using Super::w;
	using Super::setX;
	using Super::setY;
	using Super::setZ;
	using Super::setW;

	using Super::operator-;
	This operator-() const { return conjugate(); }

	using Super::operator*;
	This operator*(This _b) const
	{	return This(
			w()*_b.x() + x()*_b.w() + y()*_b.z() - z()*_b.y(),
			w()*_b.y() - x()*_b.z() + y()*_b.w() + z()*_b.x(),
			w()*_b.z() + x()*_b.y() - y()*_b.x() + z()*_b.w(),
			w()*_b.w() - x()*_b.x() - y()*_b.y() - z()*_b.z());
	}

	using Super::operator*=;
	This operator*=(This _b) { set(w()*_b.x() + x()*_b.w() + y()*_b.z() - z()*_b.y(), w()*_b.y() - x()*_b.z() + y()*_b.w() + z()*_b.x(), w()*_b.z() + x()*_b.y() - y()*_b.x() + z()*_b.w(), w()*_b.w() - x()*_b.x() - y()*_b.y() - z()*_b.z()); return *this; }

	bool operator==(This _c) const { return compare(_c); }
	bool operator!=(This _c) const { return !compare(_c); }

	static const This rotation(T _x, T _y, T _z, T _theta) { return This(_x * sin(_theta/2.0), _y * sin(_theta/2.0), _z * sin(_theta/2.0), cos(_theta/2.0)); }
	/// Vector is axis of rotation, angle is clockwise rotation around that vector looking "out"
	template<class V> static This rotation(Vector3<V> _v, T _theta)
	{	return This(
			_v.x() * sin(_theta / 2.0),
			_v.y() * sin(_theta / 2.0),
			_v.z() * sin(_theta / 2.0),
			cos(_theta / 2.0));
	}
	// Vector is "look at" direction, angle is "twist" clockwise around that vector looking "out"
	// Output is the rotation of z-axis onto target
	static Quaternion lookAt(Vector3<T> const& _lookAt, float _twist = 0.f)
	{
		// NB: z-axis is forward horizontal
		//     x-axis is right horizontal
		//     y=axis is down

		Vector3<T> v = _lookAt.normalized();
		// in degenerate case of tiny x & y, we just make it up.
		Vector3<T> axis = v.isXYSignificant() ? Vector3<T>(-v.y(), v.x(), 0.0).normalized() : Xaxis; // cross product with forward (0,0,1)
		Vector3<T> forward(0, 0, 1);
		float angle = acos(v.z()); // dot product with forward (0,0,1)
		return Quaternion::rotation(axis, angle) * Quaternion::rotation(forward, _twist);
	}

	// First vector is "look at" direction, second is target for "up"
	// Output is the rotation of z-axis onto target
	// NB: if the second vector is not perpendicular to first, the axes are
	//     rotated to get the y-axis as close as possible, but nothing more
	static Quaternion lookAt(Vector3<T> const& _v, Vector3<T> const& _up)
	{
		// get "look-at" rotation
		Quaternion q1 = lookAt(_v, 0);
		// get up vector in light coords
		Vector3<T> up = q1.conjugate().applied(_up);
		//  project up vector into x-y plane
		float x = up.x();
		float y = up.y();
		// get twist (z-axis) angle between current y and target up
		float twist = 0;
		float lenXY = sqr(x) + sqr(y);
		if (isSignificant(lenXY))
		{
			twist = acos(clamp(y / sqrt(lenXY), -1.f, 1.f)); // 0 -- pi
			if (x > 0)
				twist = TwoPi - twist;
		}

		//Quaternion q = q1 * Quaternion::rotation(Vector3<T>(0, 0, 1), twist);
		//std::cout<<"DEBUG QUATERNION: "<<_v<<"&"<<_up<<" --> "<<q.axis()<<"&"<<q.angle()*180.f/Pi<<std::endl;

		return q1 * Quaternion::rotation(Vector3<T>(0, 0, 1), twist);
	}

	Vector3<T> discardW() const { return Vector3<T>(Super::asQuad()); }

	using Super::length;
	using Super::length3;
	using Super::lengthSquared;

	This normalized() const { return This(slashed(length())); }
	This conjugate() const { return This(scaled(This(-1, -1, -1, 1))); }
	void normalize() { slash(length()); }
	void conjuggle() { scale(This(-1, -1, -1, 1)); }

	/**
	 * Quaternions can be used as a compound vector stored as a unit length vector and a length.
	 * This allows them to be used as a unit length without giving up the ability to adjust
	 * individual components. To do this, just use this method.
	 */
	static Quaternion compound(Vector3<T> _sum) { return Quaternion(_sum.normalized(), _sum.length()); }
	void changeComponent(Vector3<T> _old, Vector3<T> _cur) { scale(w()); sub(_old); sum(_cur); auto l3 = Super::length3(); set3(x() / l3, y() / l3, z() / l3, l3); }

	static bool isSignificant(float _v) { return fabs(_v) > s_tolerance; }
	bool isSignificant() const { return isSignificant(lengthSquared()); }
	bool isXYSignificant() const { return isSignificant(sqr(x()) + sqr(y())); }
	bool isNormalized() const { return !isSignificant(length() - 1.f); }

	template <class _T> friend _T& operator<<(_T& _out, This const& _me) { return _out << "[" << _me.x() << ", " << _me.y() << ", " << _me.z() << ";" << _me.w() << "]"; }

	Vector3<T> applied(Vector3<T> const& _v) const { return Quaternion((*this).normalized() * Quaternion(_v) * conjugate()).toVector3(); }
	Vector3<T> toVector3() const { return Vector3<T>(x(), y(), z()); }
	Vector3<T> axis() const { return (*this).normalized().toVector3().normalized(); }

	Vector3<T> lookAt() const { return applied(Vector3<T>(0, 0, 1)); }
	Vector3<T> up() const { return applied(Vector3<T>(0, 1, 0)); }

	float angle() const { return (*this).normalized().twiceArcCosA(); }

	bool approximates(Quaternion const& _cmp) const { return Quaternion((*this) - _cmp).lengthSquared() < s_tolerance; }

private:
	T twiceArcCosA() const { return 2 * acos(w()); }
};

typedef Quaternion<float> fQuaternion;
typedef Quaternion<double> dQuaternion;

LIGHTBOX_UNITTEST(0, "Quaternion::lookAt")
{
	LIGHTBOX_REQUIRE_APPROXIMATES(fQuaternion::lookAt(-Zaxis).lookAt(), -Zaxis);
	LIGHTBOX_REQUIRE_APPROXIMATES(fQuaternion::lookAt(fVector3(0, 0.1, -0.9)).lookAt(), fVector3(0, 0.1, -0.9).normalized());
	LIGHTBOX_REQUIRE_APPROXIMATES(fQuaternion::lookAt(fVector3(-0.0001525, 0.993884, 0.110432), fVector3(0, 0.110432, -0.93884)).lookAt(), fVector3(-0.0001525, 0.993884, 0.110432).normalized());
}

template <class _T> inline std::string toString(Quaternion<_T> const& _q)
{
	std::stringstream ss;
	ss << "Quaternion(" << _q.x() << ", " << _q.y() << ", " << _q.z() << ", " << _q.w() << ")";
	return ss.str();
}

}
