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
#include "Historical.h"

namespace lb
{

template <class _PP, class _Factor = FloatValue<95, -2>>
class Decayed: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Decayed(Scalar _factor = _Factor::value): m_factor(_factor), m_acc(0) {}
	Decayed& setFactor(Scalar _factor) { m_factor = _factor; m_acc = 0; return *this; }
	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setFactor(m_factor);
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		m_acc = std::max<Scalar>(m_acc * m_factor, _PP::get());
	}

	using _PP::changed;

	Scalar get() const { return m_acc; }

private:
	Scalar m_factor;
	Scalar m_acc;
};

template <class _PP>
class MATHS: public Historied<_PP>
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci) { _PP::init(_eci); }
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Historied<_PP>::execute(_eci, _wave);
		if (Historied<_PP>::changed())
		{
			Gaussian distro;
			distro.mean = mean(Historied<_PP>::getVector());
			distro.sigma = max(.005f, sigma(Historied<_PP>::getVector(), distro.mean));
			Scalar prob = (distro.mean > 0.01) ? normal(_PP::get(), distro) : 1.f;
			m_last = -log(prob);
		}
	}

	Scalar get() const { return m_last; }
	bool changed() const { return true; }

private:
	Scalar m_last;
};

template <class _PP, class _TroughRatio = FloatValue<1, 0> >
class TroughLimit: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ElementType ElementType;
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = m_lastTrough = zero_of<ElementType>::value();
		m_isNew = false;
	}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		m_isNew = Super::changed() && Super::getTrough() / get() < _TroughRatio::value;
		if (m_isNew)
			m_lastTrough = Super::getTrough();
	}

	using Super::get;
	ElementType const& getTrough() const { return m_lastTrough; }
	bool changed() const { return m_isNew; }

private:
	ElementType m_last;
	ElementType m_lastTrough;
	bool m_isNew;
};

}
