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

#include <vector>
#include "Global.h"
#include "Fixed.h"
#include "fix_fft.h"

struct fftwf_plan_s;

namespace Lightbox
{

#ifndef LIGHTBOX_ANDROID

class FFTW
{
public:
	explicit FFTW(unsigned _arity);
	~FFTW();

	unsigned arity() const { return m_arity; }
	unsigned bands() const { return m_arity / 2 + 1; }
    float* in() const { return m_in; }
    std::vector<float> const& mag() const { return m_mag; }
    std::vector<float> const& phase() const { return m_phase; }

	void process();

private:
	unsigned m_arity;
	float* m_in;
    float* m_work;
    std::vector<float> m_mag;
    std::vector<float> m_phase;
    fftwf_plan_s* m_plan;
};

#endif

template <class FixedType = Fixed16>
class FixedFFT
{
public:
	explicit FixedFFT(unsigned _arity): m_arity(_arity)
	{
		assert(highestBitOnly(m_arity) == m_arity);
		m_real.resize(m_arity);
		m_imag.resize(m_arity);
		m_mag.resize(m_arity / 2 + 1);
		m_phase.resize(m_arity / 2 + 1);
	}

	~FixedFFT()
	{}

	unsigned arity() const { return m_arity; }
	unsigned bands() const { return m_arity / 2 + 1; }
	int16_t* in() const { return (int16_t*)m_real.data(); }
	std::vector<FixedType> const& mag() const { return m_mag; }
	std::vector<FixedType> const& phase() const { return m_phase; }

	void process()
	{
		fix_fft(m_real.data(), m_imag.data(), log2(m_arity), 0);
		for (unsigned i = 0; i <= m_arity / 2; i++)
		{
			FixedType re = Fixed<1, 15>::from_base(m_real[i]);//(i == m_arity / 2) ? 0 : m_work[i];
			FixedType im = Fixed<1, 15>::from_base(m_imag[i]);//i ? m_work[m_arity - i] : 0;
			FixedType p = (re * re + im * im) / float(m_arity);
			FixedType m = (isFinite(p) && p != 0.f) ? sqrt(p) : 0;
			m_mag[i] = m;
			m_phase[i] = atan2(re, im) + pi<FixedType>();
			while (m_phase[i] >= twoPi<FixedType>())
				m_phase[i] -= twoPi<FixedType>();
		}
	}

private:
	unsigned m_arity;
	std::vector<int16_t> m_real;
	std::vector<int16_t> m_imag;
	std::vector<FixedType> m_mag;
	std::vector<FixedType> m_phase;
};

template <class _N> struct FFT {};
template <> struct FFT<float>
{
	typedef FFTW type;
	static int16_t* inInt16(type&) { return nullptr; }
	static float* inFloat(type& _t) { return _t.in(); }
};

template <size_t I, size_t F> struct FFT<Fixed<I, F>>
{
	typedef FixedFFT<Fixed<I, F>> type;
	static int16_t* inInt16(type& _t) { return _t.in(); }
	static float* inFloat(type&) { return nullptr; }
};

}
