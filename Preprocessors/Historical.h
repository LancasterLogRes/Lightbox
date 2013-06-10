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

template <class _PP, unsigned _defaultSize = 8>
class Historied: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Historied(unsigned _s = _defaultSize): m_data(_s) {}

	Historied& setHistory(unsigned _s) { m_data.clear(); m_data.resize(_s + 4, 0); m_count = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setHistory(m_data.size());
		m_count = 0;
	}

	void resetBefore(unsigned _bins)
	{

	}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
//		cdebug << "Historied::execute" << m_count;
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			if (m_count == 4)
			{
				memmove(m_data.data(), m_data.data() + 4, (m_data.size() - 4) * sizeof(Scalar));
				m_count = 0;
			}
			m_data[m_data.size() - 4 + m_count] = _PP::get();
			++m_count;
		}
	}

	bool changed() const { return _PP::changed() && m_count == 4; }
	foreign_vector<Scalar> get() const { return foreign_vector<Scalar>(const_cast<Scalar*>(m_data.data()) + 4, m_data.size() - 4); }
	std::vector<Scalar> const& getVector() const { return m_data; }	// note - offset by 4.

private:
	std::vector<Scalar> m_data;
	unsigned m_count;
};

// Slightly faster version of Historied - it only does a memmove every _MoveEvery inputs.
template <class _PP, unsigned _DefaultHistorySize = 0, unsigned _MoveEvery = 1>
class SlowHistoried: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	SlowHistoried(unsigned _s = _DefaultHistorySize) { setHistory(_s); }

	SlowHistoried& setHistory(unsigned _s) { m_data = std::vector<Scalar>(_s + _MoveEvery - 1, 0.0); m_offset = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
	}

	void resetBefore(unsigned _bins) {}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			++m_offset;
			if (m_offset == _MoveEvery)
			{
				valmove(m_data.data(), m_data.data() + _MoveEvery, m_data.size() - _MoveEvery);
				m_offset = 0;
			}
			m_data[m_data.size() - _MoveEvery + m_offset] = _PP::get();
		}
	}

	bool changed() const { return _PP::changed(); }
	foreign_vector<Scalar> get() const { return foreign_vector<Scalar>(const_cast<Scalar*>(m_data.data()) + m_offset, m_data.size() - _MoveEvery + 1); }

private:
	std::vector<Scalar> m_data;
	unsigned m_offset;
};

template <class _PP, unsigned _ds = 8>
class GenHistoried: public _PP
{
public:
	typedef typename Info<_PP>::ElementType ElementType;
	typedef typename Info<_PP>::ScalarType Scalar;

	GenHistoried(unsigned _s = _ds): m_data(_s) {}

	GenHistoried& setHistory(unsigned _s) { m_data.clear(); m_data.resize(_s); m_count = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setHistory(m_data.size());
	}

	void resetBefore(unsigned _bins)
	{
	}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			valmove(m_data.data(), m_data.data() + 1, m_data.size() - 1);
			m_data[m_data.size() - 1] = _PP::get();
		}
	}

	bool changed() const { return _PP::changed(); }
	foreign_vector<ElementType> get() const { return foreign_vector<ElementType>(const_cast<ElementType*>(m_data.data()), m_data.size()); }
	std::vector<ElementType> const& getVector() const { return m_data; }

private:
	std::vector<ElementType> m_data;
	unsigned m_count;
};

template <class _PP, unsigned _Size>
class TemporalGaussian: public GenHistoried<_PP, _Size>
{
public:
	typedef GenHistoried<_PP, _Size> Super;
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = zero_of<GenGaussian<typename Super::ElementType> >::value();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		if (Super::changed())
		{
			m_last.mean = mean(Super::get());
			m_last.sigma = sigma(Super::get(), m_last.mean);
		}
	}
	GenGaussian<typename Super::ElementType> const& get() const { return m_last; }

private:
	GenGaussian<typename Super::ElementType> m_last;
};

}
