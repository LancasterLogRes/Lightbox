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
class PeakPick: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ElementType ElementType;
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = m_lastButOne = m_lastTrough = zero_of<ElementType>::value();
		m_isNew = false;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto n = Super::get();
		m_isNew = (n < m_last && m_lastButOne < m_last);
		if (!m_isNew && n > m_last && m_lastButOne > m_last)
			m_lastTrough = m_last;
		m_lastButOne = m_last;
		m_last = n;
	}
	ElementType const& get() const { return m_lastButOne; }
	ElementType const& getTrough() const { return m_lastTrough; }
	bool changed() const { return m_isNew; }

private:
	ElementType m_last;
	ElementType m_lastTrough;
	ElementType m_lastButOne;
	bool m_isNew;
};

template <class _PP>
class DeltaPeak: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ElementType ElementType;
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = m_trough = m_lastInput = 0;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto l = Super::get();
		m_last = 0;
		if (l < m_lastInput)
		{
			if (l > m_trough)
				m_last = l - m_trough;
			m_trough = l;
		}
		m_lastInput = l;
	}
	ElementType const& get() const { return m_last; }
	bool changed() const { return Super::changed(); }

private:
	ElementType m_last;
	ElementType m_trough;
	ElementType m_lastInput;
};

}
