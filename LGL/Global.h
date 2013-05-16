#pragma once

#include <sstream>
#include <array>
#include <Common/Global.h>
#include "GL.h"

#undef _0
#undef _1
#undef _2
#undef _3
#undef _4
#undef _A
#undef _B
#undef _C
#undef _D
#undef _E
#undef _F
#undef _G
#undef _H
#undef _I
#undef _J
#undef _K
#undef _L
#undef _M
#undef _N
#undef _O
#undef _P
#undef _Q
#undef _R
#undef _S
#undef _T
#undef _V
#undef _W
#undef _X
#undef _Y
#undef _Z

namespace lb
{

class Attrib;
template <class _T> class Buffer;
template <class _T> class BufferFace;
class Font;
class FontFace;
class Program;
class ProgramFace;
class Shader;
class ShaderFace;
class Texture2D;
class TextureFace2D;
class Uniform;
class FramebufferFace;
class Framebuffer;

template <class _T> struct GLTypeTraits {};
template <> struct GLTypeTraits<float> { static const GLenum typeEnum = GL_FLOAT; static const unsigned _arity = 1; };
template <> struct GLTypeTraits<uint8_t> { static const GLenum typeEnum = GL_UNSIGNED_BYTE; static const unsigned _arity = 1; };
template <> struct GLTypeTraits<int8_t> { static const GLenum typeEnum = GL_BYTE; static const unsigned _arity = 1; };
template <> struct GLTypeTraits<uint16_t> { static const GLenum typeEnum = GL_UNSIGNED_SHORT; static const unsigned _arity = 1; };
template <> struct GLTypeTraits<int16_t> { static const GLenum typeEnum = GL_SHORT; static const unsigned _arity = 1; };
template <> struct GLTypeTraits<uint32_t> { static const GLenum typeEnum = GL_UNSIGNED_INT; static const unsigned _arity = 1; };
template <> struct GLTypeTraits<int32_t> { static const GLenum typeEnum = GL_INT; static const unsigned _arity = 1; };
template <class _T, size_t _z> struct GLTypeTraits<std::array<_T, _z> >: public GLTypeTraits<_T> { static const unsigned _arity = _z; };
// TODO: Vector2/3/4

static const unsigned SnoopGL = 100;
template <> struct LogName<SnoopGL> { static char const constexpr* name = "LGL"; };

void checkGlError(const char* op);
char const* glSymbolString(int _s);

template <class _T> _T GL_aux(_T _t, char const* _s) { cbug(SnoopGL) << _s << "->" << _t; checkGlError(_s); return _t; }

#define LIGHTBOX_GL_RET(X) GL_aux(X, #X)
#define LIGHTBOX_GL(X) LIGHTBOX_BLOCK_TO_STATEMENT(cbug(::lb::SnoopGL) << #X; X; checkGlError(#X);)

inline std::string argsToString(char const*) { return std::string(); }

template <class _P> inline std::string argsToString(char const*, _P _p1)
{
	std::stringstream ss;
	ss << _p1;
	return ss.str();
}

std::string argsToString(char const* _as, int _p1);
inline std::string argsToString(char const* _as, unsigned int _p1) { return argsToString(_as, (int)_p1); }

inline std::pair<std::string, char const*> splitOnParam(char const* _as)
{
	int s = 0;
	int e = 0;
	int parens = 0;
	while (isspace(_as[e]))
		++s, ++e;
	for (; _as[e] && _as[e] != ',' || parens; e++)
		if (_as[e] == '(')
			++parens;
		else if (_as[e] == ')')
			--parens;
	while (isspace(_as[e - 1]))
		--e;
	return std::make_pair(std::string(_as + s, e - s), _as[e] ? _as + e + 1 : (_as + e));
}

template <class _P, class ... _Params> inline std::string argsToString(char const* _as, _P _p1, _Params ... _rest)
{
	auto r = splitOnParam(_as);
	return argsToString(r.first.c_str(), _p1) + ", " + argsToString(r.second, _rest ...);
}

template <class _GL, class ... _Params> inline GLuint GL_aux_R(char const* _gl, char const* _args, _GL _glf, _Params ... _p)
{
	auto r = (*_glf)(_p ...);
	cbug(SnoopGL) << _gl << "(" << argsToString(_args, _p ...) << ") ->" << r;
	checkGlError(_gl);
	return r;
}

template <class _GL, class ... _Params> inline void GL_aux_N(char const* _gl, char const* _args, _GL _glf, _Params ... _p)
{
	cbug(SnoopGL) << _gl << "(" << argsToString(_args, _p ...) << ")";
	(*_glf)(_p ...);
	checkGlError(_gl);
}

#ifdef DEBUG
#define LB_GL_R(GL, ...) GL_aux_R(#GL, #__VA_ARGS__, &GL, __VA_ARGS__)
#define LB_GL(GL, ...) GL_aux_N(#GL, #__VA_ARGS__, &GL, __VA_ARGS__)
#define LB_GL_RE(GL) GL_aux_R(#GL, "", &GL)
#define LB_GL_E(GL) GL_aux_N(#GL, "", &GL)
#else
#define LB_GL_R(GL, ...) GL(__VA_ARGS__)
#define LB_GL(GL, ...) GL(__VA_ARGS__)
#define LB_GL_RE(GL) GL()
#define LB_GL_E(GL) GL()
#endif

//inline bool assert(bool _b) { return _b; }
#undef Assert
#define Assert(X) X

}
