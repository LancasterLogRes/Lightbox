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
	std::vector<Fixed16> const& mag() const { return m_mag; }
	std::vector<Fixed16> const& phase() const { return m_phase; }

	void process()
	{
		fix_fft(m_real.data(), m_imag.data(), log2(m_arity), 0);
		for (unsigned i = 0; i <= m_arity / 2; i++)
		{
			Fixed16 re = Fixed<1, 15>::from_base(m_real[i]);//(i == m_arity / 2) ? 0 : m_work[i];
			Fixed16 im = Fixed<1, 15>::from_base(m_imag[i]);//i ? m_work[m_arity - i] : 0;
			Fixed16 p = (re * re + im * im) / float(m_arity);
			Fixed16 m = (isFinite(p) && p != 0.f) ? sqrt(p) : 0;
			m_mag[i] = m;
			m_phase[i] = atan2(re, im) + Fixed16::pi();
			while (m_phase[i] >= Fixed16::pi() * Fixed16(2))
				m_phase[i] -= Fixed16::pi() * Fixed16(2);
		}
	}

private:
	unsigned m_arity;
	std::vector<int16_t> m_real;
	std::vector<int16_t> m_imag;
	std::vector<Fixed16> m_mag;
	std::vector<Fixed16> m_phase;
};

}
