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

	Quad<T>& col(unsigned _c) { if (assert(_c < 4)) return m_col[_c]; return Quad<T>(); }
	Quad<T> col(unsigned _c) const { if (assert(_c < 4)) return m_col[_c]; return Quad<T>(); }
	T* data() { return m_col[0].data(); }
	T const* data() const { return m_col[0].data(); }

	void rotate(T _th, Vector3<T> _d) { (void)_th; (void)_d; }
	void translate(Vector3<T> _m) { (void)_m; }
	void scale(Vector3<T> _s) { (void)_s; }
	void multiply(Matrix4x4<T> const& _by) { (void)_by; }

	// TODO
	Matrix4x4<T> scaled(Vector3<T> _s) const { (void)_s; return *this; }
	Matrix4x4<T> translated(Vector3<T> _d) const { (void)_d; return *this; }
	Matrix4x4<T> multiplied(Matrix4x4<T> const& _by) const { (void)_by; return *this; }

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
