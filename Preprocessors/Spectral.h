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

template <class _ScalarType = float, unsigned _SizeMultiple = 8>
class Accumulate
{
public:
	typedef _ScalarType Scalar;

	explicit Accumulate(unsigned _sizeMultiple = _SizeMultiple):
		m_sizeMultiple(_sizeMultiple)
	{
	}

	void init(EventCompilerImpl* _eci)
	{
		m_last = std::vector<Scalar>(m_sizeMultiple * fromBase(_eci->hop(), _eci->rate()), 0);
		m_windowSeconds = m_last.size() / float(_eci->rate());
	}
	void execute(EventCompilerImpl*, std::vector<Scalar> const& _wave)
	{
		valmove(m_last.data(), m_last.data() + _wave.size(), _wave.size() * (m_sizeMultiple - 1));
		valcpy(m_last.data() + _wave.size() * (m_sizeMultiple - 1), _wave.data(), _wave.size());
	}
	unsigned recordSize() const { return m_last.size(); }
	std::vector<Scalar> const& get() const { return m_last; }
	bool changed() const { return true; }
	float windowSeconds() const { return m_windowSeconds; }
	unsigned windowSize() const { return m_last.size(); }

private:
	unsigned m_sizeMultiple;

	std::vector<Scalar> m_last;
	float m_windowSeconds;
};

template <class _PP = Accumulate<>, unsigned _Function = (unsigned)HannWindow, class _FunctionParam = FloatValue<1, 0>, bool _ZeroPhase = true>
class WindowedComplexFourier: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef typename FFT<Scalar>::InputType ElementType;

	WindowedComplexFourier(WindowFunction _function = (WindowFunction)_Function, float _functionParam = _FunctionParam::value, bool _zeroPhase = _ZeroPhase):
		m_function(_function),
		m_functionParam(_functionParam),
		m_zeroPhase(_zeroPhase)
	{
	}

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		auto f = windowFunction(recordSize(), m_function, m_functionParam);
		m_window = std::vector<Scalar>(recordSize(), 0);
		for (unsigned i = 0; i < f.size(); ++i)
			m_window[i] = (Scalar)f[i];
		m_fft.setArity(f.size());
	}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);

		// Prepare waveform for FFT...
		unsigned accOffset = m_zeroPhase ? m_window.size() / 2 : 0;
		// First half (up to accOffset).
		auto w = m_window.data() + accOffset;
		auto a = _PP::get().data() + accOffset;
		auto d = m_fft.in();
		for (auto de = m_fft.in() + accOffset; d != de; ++d, ++w, ++a)
			*d = *w * *a;
		// Second half (after accOffset).
		w = m_window.data();
		a = _PP::get().data();
		for (auto de = m_fft.in() + m_window.size(); d != de; ++d, ++w, ++a)
			*d = *w * *a;
		m_fft.rawProcess();

		// Only got complex series - not yet split into mag/phase.
	}

	using Super::recordSize;
	using Super::windowSeconds;
	foreign_vector<ElementType> get() const { return foreign_vector<ElementType>(const_cast<ElementType*>(m_fft.out()), m_window.size()); }

	bool changed() const { return true; }

private:
	WindowFunction m_function;
	float m_functionParam;
	bool m_zeroPhase;

	std::vector<Scalar> m_window;
	FFT<Scalar> m_fft;
};

template <class _PP = WindowedComplexFourier<>>
class MagnitudeComponent: public _PP
{
public:
	typedef _PP Super;
	typedef typename Super::Scalar Scalar;
	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_last = std::vector<Scalar>(recordSize());
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);

		auto working = _PP::get();
		unsigned arity = working.size();

		for (unsigned i = 0; i <= arity / 2; i++)
		{
			float re = (i == arity / 2) ? 0 : working[i];
			float im = i ? working[arity - i] : 0;
			float p = (re * re + im * im) / float(arity);
			float m = (isFinite(p) && p != 0.f) ? sqrt(p) : 0;
			m_last[i] = m;
		}
	}

	unsigned recordSize() const { return _PP::recordSize() / 2 + 1; }
	using Super::windowSeconds;

	std::vector<Scalar> get() const { return m_last; }
	bool changed() const { return true; }

private:
	std::vector<Scalar> m_last;
};

template <class _PP = WindowedComplexFourier<>>
class PhaseComponent: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_last = std::vector<Scalar>(recordSize());
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);

		auto working = _PP::get();
		unsigned arity = working.size();

		for (unsigned i = 0; i <= arity / 2; i++)
		{
			float re = (i == arity / 2) ? 0 : working[i];
			float im = i ? working[arity - i] : 0;
			m_last[i] = ::atan2(re, im) + pi<float>();
			while (m_last[i] >= twoPi<float>())
				m_last[i] -= twoPi<float>();
		}
	}

	unsigned recordSize() const { return _PP::recordSize() / 2 + 1; }
	using Super::windowSeconds;

	std::vector<Scalar> get() const { return m_last; }
	bool changed() const { return true; }

private:
	std::vector<Scalar> m_last;
};

}
