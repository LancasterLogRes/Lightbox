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
#include "Maths.h"
#include "fix_fft.h"

#if !LIGHTBOX_ANDROID
struct fftwf_plan_s;
#endif

namespace Lightbox
{

template <class _N> struct FFT {};

template <size_t I, size_t F>
class FFT<Fixed<I, F>>
{
public:
	typedef Fixed<1, 15> InputType;
	typedef Fixed<I, F> OutputType;
	typedef float WorkType;

	explicit FFT(unsigned _arity = 0) { setArity(_arity); }

	void setArity(unsigned _arity)
	{
		m_arity = _arity;
		assert(highestBitOnly(m_arity) == m_arity);
		m_real.resize(m_arity);
		m_imag.resize(m_arity);
		m_mag.resize(m_arity / 2 + 1);
		m_phase.resize(m_arity / 2 + 1);
	}

	unsigned arity() const { return m_arity; }
	unsigned bands() const { return m_arity / 2 + 1; }
	Fixed<1, 15>* in() { return m_real.data(); }
	std::vector<OutputType> const& mag() const { return m_mag; }
	std::vector<OutputType> const& phase() const { return m_phase; }

	int16_t* inInt16() { return (int16_t*)in(); }
	Fixed<1, 15>* inFixed1x15() { return in(); }
	float* inFloat() { return nullptr; }

	void process()
	{
		memset(m_imag.data(), 0, m_imag.size() * sizeof(int16_t));
		fix_fft((int16_t*)m_real.data(), (int16_t*)m_imag.data(), log2(m_arity), 0);
		for (unsigned i = 0; i <= m_arity / 2; i++)
		{
			WorkType re = m_real[i];//(i == m_arity / 2) ? 0 : m_work[i];
			WorkType im = m_imag[i];//i ? m_work[m_arity - i] : 0;
			WorkType p = (re * re + im * im);
			m_mag[i] = (isFinite(p) && p != 0.f) ? sqrt(p * m_arity) : 0;
			m_phase[i] = OutputType(atan2(re, im)) + pi<OutputType>();
			while (m_phase[i] >= twoPi<OutputType>())
				m_phase[i] -= twoPi<OutputType>();
		}
	}

private:
	unsigned m_arity = 0;
	std::vector<Fixed<1, 15>> m_real;
	std::vector<Fixed<1, 15>> m_imag;
	std::vector<OutputType> m_mag;
	std::vector<OutputType> m_phase;
};

#if !LIGHTBOX_ANDROID
template <>
class FFT<float>
{
public:
	typedef float InputType;
	typedef float OutputType;

	explicit FFT(unsigned _arity = 0) { setArity(_arity); }
	~FFT() { setArity(0); }

	void setArity(unsigned _arity);

	unsigned arity() const { return m_arity; }
	unsigned bands() const { return m_arity / 2 + 1; }
    float* in() const { return m_in; }
    std::vector<float> const& mag() const { return m_mag; }
    std::vector<float> const& phase() const { return m_phase; }

	int16_t* inInt16() { return nullptr; }
	Fixed<1, 15>* inFixed1x15() { return nullptr; }
	float* inFloat() { return in(); }

	void process();

private:
	unsigned m_arity = 0;
	float* m_in = nullptr;
	float* m_work = nullptr;
    std::vector<float> m_mag;
    std::vector<float> m_phase;
	fftwf_plan_s* m_plan = nullptr;
};
#endif

typedef FFT<float> FFTW;

}
