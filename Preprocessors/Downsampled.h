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

#include "Common.h"

namespace lb
{

template <class _T> struct DownsampleHelper
{
	static void accumulate(_T& _acc, _T const& _x) { _acc += _x; }
	static void scale(_T& _acc, _T _s) { _acc *= _s; }
};
template <class _T, size_t _Z> struct DownsampleHelper<std::array<_T, _Z>>
{
	static void accumulate(std::array<_T, _Z>& _acc, std::array<_T, _Z> const& _x)
	{
		_T* la = _acc.data() + _Z;
		_T* a = _acc.data();
		_T const* x = _x.data();
		for (; a != la; ++a, ++x)
			*a += *x;
	}
	static void scale(std::array<_T, _Z>& _acc, _T _s) { for (auto& i: _acc) i *= _s; }
};

template <class _PP, unsigned _df = 1u>
class Downsampled: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef typename Info<_PP>::ElementType ElementType;

	Downsampled(unsigned _f = _df): m_factor(_f), m_acc(zero_of<ElementType>::value()), m_count(_f) {}

	Downsampled& setDownsample(unsigned _f) { m_factor = _f; m_acc = zero_of<ElementType>::value(); m_count = _f; m_scale = 1 / (Scalar)_f; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setDownsample(m_factor);
	}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (m_count == m_factor)
		{
			m_acc = _PP::get();
			m_count = 1;
		}
		else
		{
			DownsampleHelper<ElementType>::accumulate(m_acc, _PP::get());
			++m_count;
			if (m_count == m_factor)
				DownsampleHelper<ElementType>::scale(m_acc, m_scale);
		}
	}

	ElementType const& get() const { return m_acc; }
	bool changed() const { return m_count == m_factor; }

private:
	unsigned m_factor;
	ElementType m_acc;
	Scalar m_scale;
	unsigned m_count;
};

}
