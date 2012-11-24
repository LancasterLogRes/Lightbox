#pragma once

#include <array>
#include <GLES2/gl2.h>
#include <Common/Global.h>

namespace Lightbox
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

void checkGlError(const char* op);

template <class _T> _T GL_aux(_T _t, char const* _s) { cbug(69) << _s << "->" << _t; checkGlError(_s); return _t; }

#define LIGHTBOX_GL_RET(X) GL_aux(X, #X)
#define LIGHTBOX_GL(X) LIGHTBOX_BLOCK_TO_STATEMENT(cbug(69) << #X; X; checkGlError(#X);)

//inline bool assert(bool _b) { return _b; }
#define Assert(X) X

}
