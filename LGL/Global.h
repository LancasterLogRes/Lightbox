#pragma once

namespace Lightbox
{

void checkGlError(const char* op);

template <class _T> _T GL_aux(_T _t, char const* _s) { checkGlError(_s); return _t; }

#define LIGHTBOX_GL_RET(X) GL_aux(X, #X)
#define LIGHTBOX_GL(X) LIGHTBOX_BLOCK_TO_STATEMENT(X; checkGlError(#X);)

//inline bool assert(bool _b) { return _b; }
#define Assert(X) X

}
