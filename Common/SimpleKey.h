#pragma once

#include <unordered_set>
#include "Maths.h"

namespace lb
{

typedef uint32_t SimpleKey;

inline SimpleKey generateKey(SimpleKey _t) { return _t; }
inline SimpleKey generateKey(char const* _t) { return std::hash<std::string>()(_t); }
template <class _T> inline SimpleKey generateKey(_T _t) { return std::hash<_T>()(_t); }
template <class _T, class _S, class ... _O> inline SimpleKey generateKey(_T _t, _S _s, _O ... _o) { SimpleKey k = generateKey(_t); return generateKey((k << 5) + (k >> 2) + generateKey(_s), _o ...); }
inline SimpleKey generateKey(WindowFunction _t) { return (SimpleKey)_t; }


}
