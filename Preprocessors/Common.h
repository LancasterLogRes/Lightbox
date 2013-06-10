/* BEGIN COPYRIGHT
 *
 * This file is part of Noted.
 *
 * Copyright ©2011, 2012, Lancaster Logic Response Limited.
 *
 * Noted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Noted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Noted.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <algorithm>
#include <functional>
#include <tuple>
#include <deque>
#include <vector>
#include <iostream>
#include <array>
#include <Common/Common.h>
#include <EventCompiler/EventCompilerImpl.h>

namespace lb
{

inline float band(unsigned _bandCount, float _windowSeconds, float _frequency) { return clamp<float>(_frequency * _windowSeconds, 0.f, _bandCount - 1.f); }

template <int _Base, int _Exp>
struct FloatValue
{
	constexpr static float value = FloatValue<_Base, _Exp + 1>::value / 10.f;
};

template <int _Base>
struct FloatValue<_Base, 0>
{
	static constexpr float value = _Base;
};

template <class _E> struct get_scalar { typedef _E type; };
template <class _E> struct get_scalar<std::vector<_E>> { typedef typename get_scalar<_E>::type type; };
template <class _E, size_t _Z> struct get_scalar<std::array<_E, _Z>> { typedef typename get_scalar<_E>::type type; };
template <class _E> struct get_scalar<foreign_vector<_E>> { typedef typename get_scalar<_E>::type type; };
template <class _E> struct get_scalar<GenGaussian<_E>> { typedef typename get_scalar<_E>::type type; };
template <class _E> struct get_scalar<GaussianMag<_E>> { typedef typename get_scalar<_E>::type type; };

template <class _PP>
struct Info
{
	typedef typename std::remove_const<typename std::remove_reference<decltype(_PP().get())>::type>::type ElementType;
	typedef typename get_scalar<ElementType>::type ScalarType;
};

}
