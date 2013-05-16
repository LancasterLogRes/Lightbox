#pragma once

#include <memory>
#include <Numeric.h>
#include "GL.h"
#include "Texture2D.h"
#include "Global.h"

#undef Assert
#define Assert(X) X

namespace lb
{

class Program;
class ProgramFace;
class Texture2D;

class NonVolatile;
class PagedUniformFace;

template <class _Actual>
class UniformSetter
{
public:
	Texture2D const& operator=(Texture2D const& _t) const { ((_Actual*)this)->set(_t); return _t; }
	int operator=(int _i) const { ((_Actual*)this)->set(_i); return _i; }
	float operator=(float _f) const { ((_Actual*)this)->set(_f); return _f; }
	float operator=(double _v) const { float f = _v; ((_Actual*)this)->set(f); return f; }
	template<typename _T> Vector2<_T> operator=(Vector2<_T> _v) const { ((_Actual*)this)->set(_v.x(), _v.y()); return _v; }
	template<typename _T> Vector3<_T> operator=(Vector3<_T> _v) const { ((_Actual*)this)->set(_v.x(), _v.y(), _v.z()); return _v; }
	template<typename _T> Vector4<_T> operator=(Vector4<_T> _v) const { ((_Actual*)this)->set(_v.x(), _v.y(), _v.z(), _v.w()); return _v; }
	template<typename _T> Matrix4x4<_T> const& operator=(Matrix4x4<_T> const& _v) const { ((_Actual*)this)->set(_v); return _v; }
};

class Uniform: public UniformSetter<Uniform>
{
	friend class NonVolatile;
	friend class PagedUniformFace;

public:
	Uniform(): m_location(0) {}
	Uniform(Program const& _p, std::string const& _name);

	inline NonVolatile keep() const;

	void set(Texture2D const& _t) const;
	void set(int _0) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform1i, m_location, _0); }
	void set(int _0, int _1) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform2i, m_location, _0, _1); }
	void set(int _0, int _1, int _2) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform3i, m_location, _0, _1, _2); }
	void set(int _0, int _1, int _2, int _3) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform4i, m_location, _0, _1, _2, _3); }
	void set(float _0) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform1f, m_location, _0); }
	void set(float _0, float _1) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform2f, m_location, _0, _1); }
	void set(float _0, float _1, float _2) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform3f, m_location, _0, _1, _2); }
	void set(float _0, float _1, float _2, float _3) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniform4f, m_location, _0, _1, _2, _3); }
	void set(fMatrix4 const& _m4x4) const { if (m_location >= 0 && Assert(m_p.lock().get()) && Assert(isActive())) LB_GL(glUniformMatrix4fv, m_location, 1, GL_FALSE, _m4x4.data()); }

	//template <class ... Params> void setNV(Params ... _p) { Uniform& starThis = *this; setNVAux(std::make_shared<std::function<void()> >([=](){ starThis.set(_p ...); })); }
	template <class T> void setNV(T _0) const { Uniform u(*this); setNVAux(std::function<void()>([=](){ u.set(_0); })); }
	template <class T> void setNV(T _0, T _1) const { Uniform u(*this); setNVAux(std::function<void()>([=](){ u.set(_0, _1); })); }
	template <class T> void setNV(T _0, T _1, T _2) const { Uniform u(*this); setNVAux(std::function<void()>([=](){ u.set(_0, _1, _2); })); }
	template <class T> void setNV(T _0, T _1, T _2, T _3) const { Uniform u(*this); setNVAux(std::function<void()>([=](){ u.set(_0, _1, _2, _3); })); }

	using UniformSetter::operator=;

private:
	Uniform(std::weak_ptr<ProgramFace> _p, GLint _location): m_p(_p), m_location(_location) {}

	void setNVAux(std::function<void()> const& _a) const;
	bool isActive() const;

	std::weak_ptr<ProgramFace> m_p;
	GLint m_location;
};

class NonVolatile: public UniformSetter<NonVolatile>
{
public:
	explicit NonVolatile(Uniform const& _a): m_a(_a) {}

	void set(Texture2D const& _t) const { if (m_a.isActive()) m_a.set(_t); m_a.setNV(_t); }
	void set(fMatrix4 const& _m4x4) const { if (m_a.isActive()) m_a.set(_m4x4); m_a.setNV(_m4x4); }
	template <class ... Params> void set(Params ... _p) const { if (m_a.isActive()) m_a.set(_p ...); m_a.setNV(_p ...); }

	using UniformSetter::operator=;

private:
	Uniform m_a;
};

NonVolatile Uniform::keep() const { return NonVolatile(*this); }

}
