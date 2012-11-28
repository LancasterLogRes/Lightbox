#pragma once

#include <typeinfo>

#include "Vector.h"

namespace Lightbox
{

template<class T>
class Matrix4x4
{
public:
	Matrix4x4() {}
	Matrix4x4(Quad<T> _col0, Quad<T> _col1, Quad<T> _col2, Quad<T> _col3) { m_col[0] = _col0; m_col[1] = _col1; m_col[2] = _col2; m_col[3] = _col3; }
	Matrix4x4(Matrix4x4<T> const& _s) { m_col[0] = _s.m_col[0]; m_col[1] = _s.m_col[1]; m_col[2] = _s.m_col[2]; m_col[3] = _s.m_col[3]; }
	template<class _T> explicit Matrix4x4(Matrix4x4<_T> const& _s) { m_col[0] = Quad<T>(_s.col(0)); m_col[1] = Quad<T>(_s.col(1)); m_col[2] = Quad<T>(_s.col(2)); m_col[3] = Quad<T>(_s.col(3)); }

	static Matrix4x4 const& id() { static const Matrix4x4<T> s_r(Quad<T>(1, 0, 0, 0), Quad<T>(0, 1, 0, 0), Quad<T>(0, 0, 1, 0), Quad<T>(0, 0, 0, 1)); return s_r; }
	static Matrix4x4 rotation(T _th, Vector3<T> _d)
	{
		_d.normalise();
		Matrix4x4 rot;
		T* m = rot.data();
		T b = _th;
		T c = cos(b);
		T ac = 1 - c;
		T s = sin(b);
		m[0] = _d.x * _d.x * ac + c;
		m[1] = _d.x * _d.y * ac + _d.z * s;
		m[2] = _d.x * _d.z * ac - _d.y * s;
		m[4] = _d.y * _d.x * ac - _d.z * s;
		m[5] = _d.y * _d.y * ac + c;
		m[6] = _d.y * _d.z * ac + _d.x * s;
		m[8] = _d.z * _d.x * ac + _d.y * s;
		m[9] = _d.z * _d.y * ac - _d.x * s;
		m[10] = _d.z * _d.z * ac + c;
		return rot;
	}

	Quad<T>& col(unsigned _c) { if (assert(_c < 4)) return m_col[_c]; return Quad<T>(); }
	Quad<T> col(unsigned _c) const { if (assert(_c < 4)) return m_col[_c]; return Quad<T>(); }
	Quad<T> row(unsigned _r) const { if (assert(_r < 4)) { T const* d = data() + _r; return Quad<T>(d[0], d[4], d[8], d[12]); } }
	T* data() { return m_col[0].data(); }
	T const* data() const { return m_col[0].data(); }

	Matrix4x4& rotate(T _th, Vector3<T> _d) { *this = rotation(_th, _d).multiplied(*this); return *this; }
	Matrix4x4& translate(Vector3<T> _m)
	{
		Quad<T> c(_m.x(), _m.y(), _m.z(), 0);
		m_col[0] += c * m_col[0].w();
		m_col[1] += c * m_col[1].w();
		m_col[2] += c * m_col[2].w();
		m_col[3] += c * m_col[3].w();
		return *this;
	}
	Matrix4x4& scale(Vector3<T> _s)
	{
		T* d = data();
		for (int i = 0; i < 16; ++i)
			if (i % 4 < 3)
				d[i] *= _s[i % 4];
		return *this;
	}

	Matrix4x4& multiply(Matrix4x4<T> const& _by) { return (*this = multiplied(_by)); }
	Matrix4x4& rmultiply(Matrix4x4<T> const& _by) { return (*this = _by.multiplied(*this)); }

	Matrix4x4<T> scaled(Vector3<T> _s) const { return Matrix4x4<T>(*this).scale(_s); }
	Matrix4x4<T> translated(Vector3<T> _d) const { return Matrix4x4<T>(*this).translate(_d); }
	Matrix4x4<T> multiplied(Matrix4x4<T> const& _by) const
	{
		Matrix4x4<T> ret;
		for (unsigned c = 0; c < 4; ++c)
			for (unsigned r = 0; r < 4; ++r)
				ret.m_col[c].data()[r] = ((Vector4<T>&)_by.row(r)).dotted((Vector4<T>&)_by.m_col[c]);
		return *this;
	}

	Matrix4x4<T> operator+(Vector3<T> _d) const { return translated(_d); }
	Matrix4x4<T>& operator+=(Vector3<T> _d) { translate(_d); return *this; }
	Matrix4x4<T> operator*(Vector3<T> _s) const { return scaled(_s); }
	Matrix4x4<T> operator*(Matrix4x4<T> const& _by) const { return multiplied(_by); }
	Matrix4x4<T>& operator*=(Vector3<T> _s) { scale(_s); return *this; }
	Matrix4x4<T>& operator*=(Matrix4x4<T> const& _by) { multiply(_by); return *this; }

private:
	Quad<T> m_col[4];
};

typedef Matrix4x4<float> fMatrix4;
typedef Matrix4x4<double> dMatrix4;

}
