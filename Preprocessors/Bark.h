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

#include <Common/Maths.h>
#include "Common.h"
#include "Arithmetical.h"

namespace lb
{

namespace pp
{

template <class _PP, unsigned _From = 0, unsigned _To = 26>
class BarkPhonSum: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_bark.init(Super::recordSize(), Super::windowSeconds(), s_barkBands);
		m_phon.init(s_barkCentres);
		m_last = 0;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		m_last = 0;
		auto const& x = Super::get();
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last += m_phon.value(m_bark.mag(x, cb), cb);
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	BandRemapper<Scalar> m_bark;
	Phon<Scalar> m_phon;
};

template <class _PP, unsigned _From = 0, unsigned _To = 26>
class BarkPhon: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef std::array<Scalar, _To - _From> ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		// A bit ugly - we make an assumption over determining the original window size from the magnitude bands, but without general OOB dataflow, there's not much else to be done.
		m_bark.init(Super::recordSize(), Super::windowSeconds(), s_barkBands);
		m_phon.init(s_barkCentres);
		m_last.fill(0);
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last[cb - _From] = m_phon.value(m_bark.mag(_PP::get(), cb), cb);
	}
	ElementType const& get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	BandRemapper<Scalar> m_bark;
	Phon<Scalar> m_phon;
};

}

template <class _PP, unsigned _From, unsigned _To>
class GenSum<pp::BarkPhon<_PP, _From, _To>>: public pp::BarkPhon<_PP, _From, _To>
{
public:
	typedef pp::BarkPhon<_PP, _From, _To> Super;
	typedef typename Info<Super>::ScalarType ElementType;
	typedef typename Info<Super>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = 0;
		m_scale = 1 / Scalar(_To - _From) / 100;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto const& x = Super::get();
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last += x[cb];
		m_last *= m_scale;
//		m_last -= 0.5;
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	Scalar m_scale;
};

template <class _PP, unsigned _From, unsigned _To>
class DiffSum<pp::BarkPhon<_PP, _From, _To>>: public pp::BarkPhon<_PP, _From, _To>
{
public:
	typedef pp::BarkPhon<_PP, _From, _To> Super;
	typedef typename Info<Super>::ScalarType ElementType;
	typedef typename Info<Super>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = 0;
		m_in.fill(0);
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto const& x = Super::get();
		m_last *= .9;
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last += abs(x[cb] - m_in[cb]);
		m_in = x;
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	typename Super::ElementType m_in;
	ElementType m_last;
};

}
