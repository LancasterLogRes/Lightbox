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

namespace lb
{

template <class _PP = MagnitudeComponent<> >
class Centroid: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto mag = Super::get();

		m_last = 0;
		Scalar total = 0;
		for (unsigned i = 0; i < b; ++i)
			total += sqr(mag[i]), m_last += sqr(mag[i]) * i;
		if (total)
			m_last /= total;
		m_last = (m_last + 1) / b;
		if (m_last)
			m_last = log(m_last) / log(1.0 / b);
	}

	Scalar get() const { return m_last; }
	bool changed() const { return true; }

private:
	Scalar m_last;
};

template <class _PP = PhaseComponent<> >
class PhaseUnity: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_buffer.clear();
		m_last = 0;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto phase = Super::get();

		if (_eci->time() > 0)
		{
			float currentPhaseUnity = 0;
			for (unsigned i = 1; i < b; ++i)
				currentPhaseUnity += withReflection(abs(phase[i] - phase[i - 1]));
			currentPhaseUnity /= Super::recordSize() * pi<Scalar>();
			m_buffer.push_back(currentPhaseUnity);
			if (m_buffer.size() > std::max<unsigned>(1, Super::windowSize() / _eci->hop() / 2 - 1))
			{
				m_last = std::max(0.f, m_buffer.front() - currentPhaseUnity);
				m_buffer.pop_front();
			}
		}
	}


	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	std::deque<float> m_buffer;
	float m_last;
};

template <class _PP = MagnitudeComponent<> >
class HighEnergy: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto mag = Super::get();
		m_last = 0.f;
		for (unsigned j = 0; j < b; ++j)
			m_last += (j + 1) * mag[j];
		m_last = sqr(m_last / (b * b / 2)) * 100;
	}

	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	float m_last;
};

template <class _PP = MagnitudeComponent<> >
class Harmony: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_fftw = std::shared_ptr<FFTW>(new FFTW(Super::recordSize() - 1));
		m_wF = windowFunction(m_fftw->arity(), HannWindow);
		m_last.clear();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto mag = Super::get();

		unsigned arity = m_fftw->arity();
		for (unsigned i = 0; i < arity; ++i)
			m_fftw->in()[i] = mag[(i + arity / 2) % arity] * m_wF[(i + arity / 2) % arity];
		m_fftw->process();

		std::vector<Scalar> const& metaMag = m_fftw->mag();
		if (m_last.size() == metaMag.size())
			m_d = packCombine(metaMag.begin(), m_last.begin(), metaMag.size(), [](v4sf& ret, v4sf const& a, v4sf const& b) { ret = ret + (a - b); }, [&](float const* c) { return (c[0] + c[1] + c[2] + c[3]) / metaMag.size() / 1000; });
		else
			m_d = 0;
		m_last = metaMag;
	}

	float get() const { return m_d; }
	bool changed() const { return true; }

private:
	std::shared_ptr<FFTW> m_fftw;
	std::vector<float> m_wF;
	std::vector<float> m_last;
	float m_d;
};

template <class _PP = MagnitudeComponent<> >
class LowEnergy: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto _mag = Super::get();

		m_last = 0.f;
		for (unsigned j = 0; j < b; ++j)
			m_last += _mag[j] / std::max(1, int(j) - 5);
		m_last = sqr(m_last) * (b * b / 2) / 10000000;
	}

	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	float m_last;
};

template <class _PP = MagnitudeComponent<> >
class Energy: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto _mag = Super::get();

		m_last = 0.f;
		for (unsigned j = 0; j < b; ++j)
			m_last += _mag[j];
		m_last /= b;
	}

	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	float m_last;
};

template <class _PP = MagnitudeComponent<> >
class DeltaEnergy: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_lastMag.clear();
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		unsigned b = Super::recordSize();
		auto _mag = Super::get();

		m_last = 0.f;
		if (m_lastMag.size())
		{
			for (unsigned j = 0; j < b; ++j)
				m_last += abs(_mag[j] - m_lastMag[j]);
			m_last *= 10.f / b;
		}
		m_lastMag = _mag;
	}

	bool changed() const { return true; }

	float get() const { return m_last; }

private:
	float m_last;
	std::vector<float> m_lastMag;
};

template <class _PP = MagnitudeComponent<>, unsigned _FromHz = 0, unsigned _ToHz = 96000>
class BandProfile: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef GaussianMag<Scalar> ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_from = band(Super::recordSize(), Super::windowSeconds(), _FromHz);
		m_to = band(Super::recordSize(), Super::windowSeconds(), _ToHz);
		m_last.mean = 0.f;
		m_last.mag = 0.f;
		m_last.sigma = 0.f;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto _mag = Super::get();

		m_last.mean = 0.f;
		m_last.mag = 0.f;
		m_last.sigma = 0.f;
		Scalar p = Scalar(1) / (m_to - m_from);
		Scalar j = 0;
		for (unsigned i = m_from; i < m_to; ++i, j+=p)
			m_last.mag += sqr(_mag[i]), m_last.mean += sqr(_mag[i]) * j;
		if (m_last.mag != 0)
			m_last.mean /= m_last.mag;

		j = 0;
		for (unsigned i = m_from; i < m_to; ++i, j+=p)
			m_last.sigma += sqr(sqr(_mag[i]) * (j - m_last.mean));
		if (m_last.mag != 0)
			m_last.sigma = sqrt(m_last.sigma / m_last.mag);
	}

	ElementType const& get() const { return m_last; }
	bool changed() const { return true; }

private:
	unsigned m_from;
	unsigned m_to;
	ElementType m_last;
};

template <class _PP, class _Freq>
class CutOff: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ElementType ElementType;
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = zero_of<ElementType>::value();
		m_changed = false;
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		if (Super::changed())
		{
			m_last = Super::get();
			if (m_last < _Freq::value)
			{
				m_last = zero_of<ElementType>::value();
				m_changed = true;
			}
		}
	}
	ElementType get() const { return m_last; }
	bool changed() const { return m_changed; }

private:
	ElementType m_last;
	bool m_changed;
};

template <class _PP = MagnitudeComponent<>, unsigned _FromHz = 0, unsigned _ToHz = 96000>
class BandwiseMagIncrease: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = 0;
		m_from = band(Super::recordSize(), Super::windowSeconds(), _FromHz);
		m_to = band(Super::recordSize(), Super::windowSeconds(), _ToHz);
		m_lastIn.clear();
	}

	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto _mag = Super::get();

		m_last = 0;
		auto* li = m_lastIn.data();
		if (m_lastIn.size())
			for (unsigned b = m_from; b < m_to; ++b, ++li)
			{
				m_last += sqr(std::max<Scalar>(0, _mag[b] - *li));
				*li = _mag[b];
			}
		else
		{
			m_lastIn.reserve(m_to - m_from);
			for (unsigned b = m_from; b < m_to; ++b)
				m_lastIn.push_back(_mag[b]);
		}
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	std::vector<Scalar> m_lastIn;
	unsigned m_from;
	unsigned m_to;
};

template <class _PP = MagnitudeComponent<>, unsigned _FromHz = 0, unsigned _ToHz = 96000>
class Power: public _PP
{
public:
	typedef _PP Super;
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = 0;
		m_from = band(Super::recordSize(), Super::windowSeconds(), _FromHz);
		m_to = band(Super::recordSize(), Super::windowSeconds(), _ToHz);
	}
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _wave);
		auto _mag = Super::get();

		m_last = 0;
		for (unsigned i = m_from; i < m_to; ++i)
			m_last += sqr(_mag[i]);
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	unsigned m_from;
	unsigned m_to;
};

}
