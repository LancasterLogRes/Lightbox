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
#include "Spectral.h"
#include "Bark.h"
#include "Surface.h"
#include "Downsampled.h"

namespace lb
{

template <class _PPMag = MagnitudeComponent<>, class _PPPhase = PhaseComponent<>, unsigned _FromHz = 0, unsigned _ToHz = 96000, bool _Invert = false>
class TonalPower: public _PPMag, public _PPPhase
{
public:
	typedef _PPMag MagSuper;
	typedef _PPPhase PhaseSuper;
	typedef typename Info<_PPMag>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		MagSuper::init(_eci);
		PhaseSuper::init(_eci);

		assert((std::is_same<typename Info<_PPMag>::ScalarType, typename Info<_PPPhase>::ScalarType>::value));
		assert(MagSuper::recordSize() == PhaseSuper::recordSize());
		assert(MagSuper::windowSeconds() == PhaseSuper::windowSeconds());

		m_last = 0;
		m_from = band(MagSuper::recordSize(), MagSuper::windowSeconds(), _FromHz);
		m_to = band(MagSuper::recordSize(), MagSuper::windowSeconds(), _ToHz);
		m_lphase.clear();
		m_phase.clear();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		MagSuper::execute(_eci, _wave);
		PhaseSuper::execute(_eci, _wave);
		auto _mag = MagSuper::get();
		auto _phase = PhaseSuper::get();

		m_last = 0;
		if (m_lphase.size())
		{
			for (unsigned i = m_from; i < m_to; ++i)
			{
				m_last += sqr(((_Invert ? 1.f : 0.f) + (_Invert ? -1.f : 1.f) * abs(mod1(_phase[i] - m_phase[i] + 1) - mod1(m_phase[i] - m_lphase[i] + 1))) * _mag[i]);
				m_lphase[i - m_from] = m_phase[i - m_from];
				m_phase[i - m_from] = _phase[i];
			}
		}
		else if (m_phase.size())
		{
			m_lphase.reserve(m_to - m_from);
			for (unsigned i = m_from; i < m_to; ++i)
			{
				m_lphase.push_back(m_phase[i - m_from]);
				m_phase[i - m_from] = _phase[i];
			}
		}
		else
		{
			m_phase.reserve(m_to - m_from);
			for (unsigned i = m_from; i < m_to; ++i)
				m_phase.push_back(_phase[i]);
		}
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	unsigned m_from;
	unsigned m_to;
	std::vector<Scalar> m_phase;
	std::vector<Scalar> m_lphase;
};

template <class _PPMag = MagnitudeComponent<>, class _PPPhase = PhaseComponent<>, unsigned _FromHz = 0, unsigned _ToHz = 96000>
class NonTonalPower: public TonalPower<_PPMag, _PPPhase, _FromHz, _ToHz> {};

}
