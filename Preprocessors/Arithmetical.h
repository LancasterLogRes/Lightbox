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

template <class _PP>
class Deltad: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_last = 0;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			m_delta = abs(_PP::get() - m_last);
			m_last = _PP::get();
		}
	}
	using _PP::changed;

	Scalar get() const { return m_delta; }

private:
	Scalar m_last;
	Scalar m_delta;
};

template <class _PP1, class _PP2>
class Subbed: public _PP1, public _PP2
{
public:
	typedef typename Info<_PP1>::ScalarType Scalar;
	typedef typename Info<_PP1>::ElementType TT;

	void init(EventCompilerImpl* _eci)
	{
		_PP1::init(_eci);
		_PP2::init(_eci);
		m_last = 0;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP1::execute(_eci, _wave);
		_PP2::execute(_eci, _wave);
		if (_PP1::changed() || _PP2::changed())
			m_last = _PP1::get() - _PP2::get();
	}
	bool changed() const { return _PP1::changed() || _PP2::changed(); }

	TT const& get() const { return m_last; }

private:
	TT m_last;
};

template <class _PP, unsigned _factorX1000000 = 1000000>
class Scaled: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef typename Info<_PP>::ElementType TT;

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_last = 0;
		m_scale = _factorX1000000 / (Scalar)1000000;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		m_last = _PP::get() * m_scale;
	}
	bool changed() const { return _PP::changed(); }

	TT const& get() const { return m_last; }

private:
	TT m_last;
	Scalar m_scale;
};

template <class _PP, size_t _N = 8>
class MinusLast: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci) { _PP::init(_eci); m_mean = 0; m_lastN.fill(0); m_scale = 1 / Scalar(_N); }
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			Scalar l = _PP::get();
			m_last = l - m_mean;
			m_mean += (l - m_lastN[0]) * m_scale;
			valmove(&m_lastN[0], &m_lastN[1], _N - 1);
			m_lastN[_N - 1] = l;
		}
	}

	Scalar get() const { return m_last; }
	bool changed() const { return true; }

private:
	Scalar m_last;
	Scalar m_mean;
	std::array<Scalar, _N> m_lastN;
	Scalar m_scale;
};

template <class _PP>
class GenSum: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			m_last = 0;
			for (auto i: _PP::get())
				m_last += i;
		}
	}

	bool changed() const { return _PP::changed(); }
	ElementType get() const { return m_last; }

private:
	ElementType m_last;
};

template <class _PP>
class DiffSum: public _PP
{
};

}
