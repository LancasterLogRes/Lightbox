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
class AreaNormalized: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	AreaNormalized() {}

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_data.clear();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			m_data = _PP::get();
			assert(isFinite(m_data[0]));
			makeTotalUnit(m_data);
			assert(isFinite(m_data[0]));
		}
	}
	using _PP::changed;

	std::vector<Scalar> const& get() const { return m_data; }

private:
	std::vector<Scalar> m_data;
};

template <class _PP>
class RangeAndAreaNormalized: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	RangeAndAreaNormalized() {}

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_data.clear();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			m_data = _PP::get();
			assert(isFinite(m_data[0]));
			normalize(m_data);
			assert(isFinite(m_data[0]));
			makeTotalUnit(m_data);
			assert(isFinite(m_data[0]));
		}
	}
	using _PP::changed;

	std::vector<Scalar> const& get() const { return m_data; }

private:
	std::vector<Scalar> m_data;
};

template <class _PP>
class RangeNormalized: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	RangeNormalized() {}

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_data.clear();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			m_data = _PP::get();
			normalize(m_data);
		}
	}
	using _PP::changed;

	std::vector<Scalar> const& get() const { return m_data; }

private:
	std::vector<Scalar> m_data;
};

}
