#pragma once

#include <memory>
#include <Numeric.h>
#include <GLES2/gl2.h>
#include "Global.h"

namespace Lightbox
{

class Program;
class ProgramFace;
class Texture;

class Uniform
{
public:
	Uniform(): m_location(0) {}
	Uniform(Program const& _p, std::string const& _name);

	int location() const { return m_location; }

//	Texture const& operator=(Texture const& _t) { set(_t); return _t; }
	int operator=(int _i) { set(_i); return _i; }
	float operator=(float _f) { set(_f); return _f; }
	float operator=(double _v) { float f = _v; set(f); return f; }
	template<typename _T> Vector2<_T> operator=(Vector2<_T> _v) { set(_v.x(), _v.y()); return _v; }
	template<typename _T> Vector3<_T> operator=(Vector3<_T> _v) { set(_v.x(), _v.y(), _v.z()); return _v; }
	template<typename _T> Vector4<_T> operator=(Vector4<_T> _v) { set(_v.x(), _v.y(), _v.z(), _v.w()); return _v; }
	template<typename _T> Matrix4x4<_T> const& operator=(Matrix4x4<_T> const& _v) { set(_v.data()); return _v; }

//	void set(Texture const& _t);
	void set(int _0) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform1i(m_location, _0)); }
	void set(int _0, int _1) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform2i(m_location, _0, _1)); }
	void set(int _0, int _1, int _2) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform3i(m_location, _0, _1, _2)); }
	void set(int _0, int _1, int _2, int _3) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform4i(m_location, _0, _1, _2, _3)); }
	void set(float _0) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform1f(m_location, _0)); }
	void set(float _0, float _1) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform2f(m_location, _0, _1)); }
	void set(float _0, float _1, float _2) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform3f(m_location, _0, _1, _2)); }
	void set(float _0, float _1, float _2, float _3) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniform4f(m_location, _0, _1, _2, _3)); }
	void set(float const* _m4x4) { if (m_location >= 0 && Assert(m_p.lock().get())) LIGHTBOX_GL(glUniformMatrix4fv(m_location, 1, GL_FALSE, _m4x4)); }

private:
	std::weak_ptr<ProgramFace> m_p;
	GLint m_location;
};

#define LIGHTBOX_USING_UNIFORM(_name) Uniform _name(Program::inUse(), #_name)

}
