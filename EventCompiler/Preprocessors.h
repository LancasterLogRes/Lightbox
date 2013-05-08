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

#include <algorithm>
#include <functional>
#include <tuple>
#include <deque>
#include <vector>
#include <iostream>
#include <array>
#include <boost/circular_buffer.hpp>
#include <Common/Common.h>
#include <EventCompiler/EventCompilerImpl.h>
using namespace std;
using boost::circular_buffer;
using namespace Lightbox;

namespace Lightbox
{
/*

	vector<float> lastPhase;
	vector<float> deltaPhase;

: phaseEntropyAvg(12),

deltaPhase.resize(bands(), -1);
lastPhase.resize(bands(), -1);
phaseEntropyAvg.init();
phaseEntropyC.setHistory(c_historySize, downsampleST);
*/
/*
		float phaseEntropy = 0.f;
		if (lastPhase[0] == -1)
			lastPhase = _phase;
		else if (deltaPhase[0] == -1)
			for (unsigned i = 0; i < bands(); ++i)
				deltaPhase[i] = withReflection(abs(_phase[i] - lastPhase[i]));
		else
		{
			for (unsigned i = 0; i < bands(); ++i)
			{
				float dp = withReflection(abs(_phase[i] - lastPhase[i]));
				lastPhase[i] = _phase[i];
				phaseEntropy += max<float>(0, withReflection(abs(dp - deltaPhase[i])) - halfPi<float>());
				deltaPhase[i] = dp;
			}
			ret.push_back(StreamEvent(Graph, phaseEntropy, 0.08f));
		}
		phaseEntropyAvg.push(phaseEntropy);
*/

template <class _N = float>
struct GaussianMag
{
	GaussianMag(): mean(0), sigma(0), mag(0) {}
	GaussianMag(GaussianMag const& _s): mean(_s.mean), sigma(_s.sigma), mag(_s.mag) {}
	GaussianMag(_N _mean, _N _sigma, _N _mag): mean(_mean), sigma(_sigma), mag(_mag) {}
	LIGHTBOX_STRUCT_INTERNALS_3 (GaussianMag, _N, mean, _N, sigma, _N, mag)

	_N mean;
	_N sigma;
	_N mag;
};

template <class _N> GaussianMag<_N> sqrt(GaussianMag<_N> const& _a) { return GaussianMag<_N>(sqrt(_a.mean), sqrt(_a.sigma), sqrt(_a.mag)); }
template <class _N> struct zero_of<GaussianMag<_N>> { static GaussianMag<_N> value() { return GaussianMag<_N>(_N(0), _N(0), _N(0)); } };
template <class _N> GaussianMag<_N>& operator+=(GaussianMag<_N>& _a, GaussianMag<_N> const& _b) { _a.mean += _b.mean; _a.sigma += _b.sigma; _a.mag += _b.mag; return _a; }
template <class _N, class _M> GaussianMag<_N> operator/(GaussianMag<_N> const& _a, _M _b) { return GaussianMag<_N>(_a.mean / _b, _a.sigma / _b, _a.mag / _b); }
template <class _N> GaussianMag<_N> operator*(GaussianMag<_N> const& _a, GaussianMag<_N> const& _b) { return GaussianMag<_N>(_a.mean * _b.mean, _a.sigma * _b.sigma, _a.mag * _b.mag); }
template <class _N> GaussianMag<_N> operator-(GaussianMag<_N> const& _a, GaussianMag<_N> const& _b) { return GaussianMag<_N>(_a.mean - _b.mean, _a.sigma - _b.sigma, _a.mag - _b.mag); }

template <int _Base, int _Exp>
struct FloatValue
{
	constexpr static float value = FloatValue<_Base, _Exp + 1>::value / 10.f;
};

template <int _Base>
struct FloatValue<_Base, 0>
{
	static constexpr float value = _Base;
};

template <class _E> struct get_scalar { typedef _E type; };
template <class _E> struct get_scalar<vector<_E>> { typedef typename get_scalar<_E>::type type; };
template <class _E, size_t _Z> struct get_scalar<array<_E, _Z>> { typedef typename get_scalar<_E>::type type; };
template <class _E> struct get_scalar<foreign_vector<_E>> { typedef typename get_scalar<_E>::type type; };
template <class _E> struct get_scalar<GenGaussian<_E>> { typedef typename get_scalar<_E>::type type; };
template <class _E> struct get_scalar<GaussianMag<_E>> { typedef typename get_scalar<_E>::type type; };

template <class _PP>
struct Info
{
	typedef typename std::remove_const<typename std::remove_reference<decltype(_PP().get())>::type>::type ElementType;
	typedef typename get_scalar<ElementType>::type ScalarType;
};

template <class _N = float>
class PhaseUnity
{
public:
	typedef _N Scalar;

	void init(EventCompilerImpl*)
	{
		m_buffer.clear();
		m_last = 0;
	}

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const&, vector<Scalar> const& _phase, std::vector<Scalar> const&)
	{
		if (_t > _eci->windowSize())
		{
			unsigned b = _eci->bands();
			float currentPhaseUnity = 0;
			for (unsigned i = 1; i < b; ++i)
				currentPhaseUnity += withReflection(abs(_phase[i] - _phase[i - 1]));
			currentPhaseUnity /= _eci->bands() * pi<Scalar>();
			m_buffer.push_back(currentPhaseUnity);
			if (m_buffer.size() > max<unsigned>(1, _eci->windowSize() / _eci->hop() / 2 - 1))
			{
				m_last = max(0.f, m_buffer.front() - currentPhaseUnity);
				m_buffer.pop_front();
			}
		}
	}


	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	deque<float> m_buffer;
	float m_last;
};

template <class _N = float>
class HighEnergy
{
public:
	typedef _N Scalar;

	void init(EventCompilerImpl*) {}
	void execute(EventCompilerImpl* _eci, Time, vector<Scalar> const& _mag, vector<Scalar> const&, std::vector<Scalar> const&)
	{
		unsigned b = _eci->bands();
		m_last = 0.f;
		for (unsigned j = 0; j < b; ++j)
			m_last += (j + 1) * _mag[j];
		m_last = sqr(m_last / (b * b / 2)) * 100;
	}

	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	float m_last;
};

template <class _N = float>
class Harmony
{
public:
	typedef _N Scalar;
	void init(EventCompilerImpl* _eci)
	{
		m_fftw = shared_ptr<FFTW>(new FFTW(_eci->bands() - 1));
		m_wF = windowFunction(m_fftw->arity(), HannWindow);
		m_last.clear();
	}
	void execute(EventCompilerImpl*, Time, vector<Scalar> const& _mag, vector<Scalar> const&, std::vector<Scalar> const&)
	{
		unsigned arity = m_fftw->arity();
		for (unsigned i = 0; i < arity; ++i)
			m_fftw->in()[i] = _mag[(i + arity / 2) % arity] * m_wF[(i + arity / 2) % arity];
		m_fftw->process();

		vector<Scalar> const& mag = m_fftw->mag();
		if (m_last.size() == mag.size())
			m_d = packCombine(mag.begin(), m_last.begin(), mag.size(), [](v4sf& ret, v4sf const& a, v4sf const& b) { ret = ret + (a - b); }, [&](float const* c) { return (c[0] + c[1] + c[2] + c[3]) / mag.size() / 1000; });
		else
			m_d = 0;
		m_last = mag;
	}

	float get() const { return m_d; }
	bool changed() const { return true; }

private:
	std::shared_ptr<FFTW> m_fftw;
	std::vector<float> m_wF;
	std::vector<float> m_last;
	float m_d;
};

template <class _N = float>
class LowEnergy
{
public:
	typedef _N Scalar;
	void init(EventCompilerImpl*) {}
	void execute(EventCompilerImpl* _eci, Time, vector<Scalar> const& _mag, vector<Scalar> const&, std::vector<Scalar> const&)
	{
		unsigned b = _eci->bands();
		m_last = 0.f;
		for (unsigned j = 0; j < b; ++j)
			m_last += _mag[j] / max(1, int(j) - 5);
		m_last = sqr(m_last) * (b * b / 2) / 10000000;
	}

	float get() const { return m_last; }
	bool changed() const { return true; }

private:
	float m_last;
};

template <class _N = float>
class Energy
{
public:
	typedef _N Scalar;
	void init(EventCompilerImpl*) {}
	void execute(EventCompilerImpl* _eci, Time, vector<Scalar> const& _mag, vector<Scalar> const&, std::vector<Scalar> const&)
	{
		unsigned b = _eci->bands();
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

template <class _N = float>
class DeltaEnergy
{
public:
	typedef _N Scalar;
	void init(EventCompilerImpl*)
	{
		m_lastMag.clear();
	}

	void execute(EventCompilerImpl* _eci, Time, vector<Scalar> const& _mag, vector<Scalar> const&, std::vector<Scalar> const&)
	{
		unsigned b = _eci->bands();
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
	vector<float> m_lastMag;
};

template <class _PP, unsigned _factorX1000000 = 950000>
class Decayed: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Decayed(Scalar _factor = _factorX1000000 / 1000000.f): m_factor(_factor), m_acc(0) {}
	Decayed& setFactor(Scalar _factor) { m_factor = _factor; m_acc = 0; return *this; }
	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setFactor(m_factor);
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		m_acc = max<Scalar>(m_acc * m_factor, _PP::get());
	}

	using _PP::changed;

	Scalar get() const { return m_acc; }

private:
	Scalar m_factor;
	Scalar m_acc;
};

template <class _T> struct DownsampleHelper
{
	static void accumulate(_T& _acc, _T const& _x) { _acc += _x; }
	static void scale(_T& _acc, _T _s) { _acc *= _s; }
};
template <class _T, size_t _Z> struct DownsampleHelper<array<_T, _Z>>
{
	static void accumulate(array<_T, _Z>& _acc, array<_T, _Z> const& _x)
	{
		_T* la = _acc.data() + _Z;
		_T* a = _acc.data();
		_T const* x = _x.data();
		for (; a != la; ++a, ++x)
			*a += *x;
	}
	static void scale(array<_T, _Z>& _acc, _T _s) { for (auto& i: _acc) i *= _s; }
};

template <class _PP, unsigned _df = 1u>
class Downsampled: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef typename Info<_PP>::ElementType ElementType;

	Downsampled(unsigned _f = _df): m_factor(_f), m_acc(zero_of<ElementType>::value()), m_count(_f) {}

	Downsampled& setDownsample(unsigned _f) { m_factor = _f; m_acc = zero_of<ElementType>::value(); m_count = _f; m_scale = 1 / (Scalar)_f; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setDownsample(m_factor);
	}

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (m_count == m_factor)
		{
			m_acc = _PP::get();
			m_count = 1;
		}
		else
		{
			DownsampleHelper<ElementType>::accumulate(m_acc, _PP::get());
			++m_count;
			if (m_count == m_factor)
				DownsampleHelper<ElementType>::scale(m_acc, m_scale);
		}
	}

	ElementType const& get() const { return m_acc; }
	bool changed() const { return m_count == m_factor; }

private:
	unsigned m_factor;
	ElementType m_acc;
	Scalar m_scale;
	unsigned m_count;
};

template <class _PP, unsigned _ds = 8>
class Historied: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Historied(unsigned _s = _ds): m_data(_s) {}

	Historied& setHistory(unsigned _s) { m_data.clear(); m_data.resize(_s + 4, 0); m_count = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setHistory(m_data.size());
		m_count = 0;
	}

	void resetBefore(unsigned _bins)
	{

	}

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
//		cdebug << "Historied::execute" << m_count;
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			if (m_count == 4)
			{
				memmove(m_data.data(), m_data.data() + 4, (m_data.size() - 4) * sizeof(Scalar));
				m_count = 0;
			}
			m_data[m_data.size() - 4 + m_count] = _PP::get();
			++m_count;
		}
	}

	bool changed() const { return _PP::changed() && m_count == 4; }
	foreign_vector<Scalar> get() const { return foreign_vector<Scalar>(const_cast<Scalar*>(m_data.data()) + 4, m_data.size() - 4); }
	vector<Scalar> const& getVector() const { return m_data; }	// note - offset by 4.

private:
	vector<Scalar> m_data;
	unsigned m_count;
};

// Slightly faster version of Historied - it only does a memmove every _MoveEvery inputs.
template <class _PP, unsigned _DefaultHistorySize = 0, unsigned _MoveEvery = 1>
class SlowHistoried: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	SlowHistoried(unsigned _s = _DefaultHistorySize) { setHistory(_s); }

	SlowHistoried& setHistory(unsigned _s) { m_data = vector<Scalar>(_s + _MoveEvery - 1, 0.5); m_offset = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
	}

	void resetBefore(unsigned _bins) {}

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			m_data[m_data.size() - _MoveEvery + m_offset] = _PP::get();
			++m_offset;
			if (m_offset == _MoveEvery)
			{
				valmove(m_data.data(), m_data.data() + _MoveEvery, m_data.size() - _MoveEvery);
				m_offset = 0;
			}
		}
	}

	bool changed() const { return _PP::changed(); }
	foreign_vector<Scalar> get() const { return foreign_vector<Scalar>(const_cast<Scalar*>(m_data.data()) + m_offset, m_data.size() - _MoveEvery); }

private:
	vector<Scalar> m_data;
	unsigned m_offset;
};

template <class _PP, unsigned _ds = 8>
class GenHistoried: public _PP
{
public:
	typedef typename Info<_PP>::ElementType ElementType;
	typedef typename Info<_PP>::ScalarType Scalar;

	GenHistoried(unsigned _s = _ds): m_data(_s) {}

	GenHistoried& setHistory(unsigned _s) { m_data.clear(); m_data.resize(_s); m_count = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		setHistory(m_data.size());
	}

	void resetBefore(unsigned _bins)
	{
	}

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			valmove(m_data.data(), m_data.data() + 1, m_data.size() - 1);
			m_data[m_data.size() - 1] = _PP::get();
		}
	}

	bool changed() const { return _PP::changed(); }
	foreign_vector<ElementType> get() const { return foreign_vector<ElementType>(const_cast<ElementType*>(m_data.data()), m_data.size()); }
	vector<ElementType> const& getVector() const { return m_data; }

private:
	vector<ElementType> m_data;
	unsigned m_count;
};

template <class _PP>
class GenSum: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef Scalar ElementType;

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			m_last = 0;
			for (auto i: _PP::get())
				m_last += i;
		}
	}

	bool changed() const { return _PP::changed(); }
	ElementType get() const { return m_last; }

private:
	ElementType m_last;
};

template <class _PP, class _X>
class DirectCrossed: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_lastAc.clear();
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			auto const& h = _PP::get();
			assert(isFinite(h[0]));
			unsigned s = h.size();
			if (m_lastAc.empty())
				m_lastAc.resize(s / 2, 0.5);
			autocross(h.begin(), s, _X::call, s / 2, 1, m_lastAc);
//			m_lastAc = autocross(h.begin(), s, _X::call, s / 2);
			assert(isFinite(m_lastAc[0]));
		}
	}

	vector<Scalar> const& get() const { return m_lastAc; }
	using _PP::changed;

	unsigned best() const
	{
		unsigned ret = 2;
		for (unsigned i = 3; i < m_lastAc.size(); ++i)
			if (m_lastAc[ret] < m_lastAc[i])
				ret = i;
		return ret;
	}

private:
	vector<Scalar> m_lastAc;
};

template <class _PP, class _X>
class Crossed: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_lastAc.clear();
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			auto const& h = _PP::getVector();
			assert(isFinite(h[0]));
			unsigned s = h.size() - 4;
			if (m_lastAc.empty())
				m_lastAc.resize(s / 4, 0.0001);
			autocross(h.begin(), s, _X::call, s / 4, 4, m_lastAc);
			assert(isFinite(m_lastAc[0]));
		}
	}

	vector<Scalar> const& get() const { return m_lastAc; }
	using _PP::changed;

	unsigned best() const
	{
		unsigned ret = 2;
		for (unsigned i = 3; i < m_lastAc.size(); ++i)
			if (m_lastAc[ret] < m_lastAc[i])
				ret = i;
		return ret;
	}

private:
	vector<Scalar> m_lastAc;
};

template <class _N = float> struct CallSimilarity { inline static _N call(typename vector<_N>::const_iterator _a, typename vector<_N>::const_iterator _b, unsigned _s) { return similarity(_a, _b, _s); } };
template <class _N = float> struct CallCorrelation { inline static _N call(typename vector<_N>::const_iterator _a, typename vector<_N>::const_iterator _b, unsigned _s) { return correlation<_N>(_a, _b, _s); } };
template <class _N = float> struct CallDissimilarity { inline static _N call(typename vector<_N>::const_iterator _a, typename vector<_N>::const_iterator _b, unsigned _s) { return dissimilarity(_a, _b, _s); } };
template <class _N = float> struct CallCorrelationPtr { inline static _N call(_N const* _a, _N const* _b, unsigned _s) { return correlation(_a, _b, _s); } };

/*
template <class _PP> using Correlated = Crossed<_PP, CallCorrelation>;
template <class _PP> using Similarity = Crossed<_PP, CallSimilarity>;
template <class _PP> using Dissimilarity = Crossed<_PP, CallDissimilarity>;
*/
template <class _PP> class Correlated: public Crossed<_PP, CallCorrelation<typename Info<_PP>::ScalarType>> {};
template <class _PP> class Similarity: public Crossed<_PP, CallSimilarity<typename Info<_PP>::ScalarType>> {};
template <class _PP> class Dissimilarity: public Crossed<_PP, CallDissimilarity<typename Info<_PP>::ScalarType>> {};
template <class _PP> class CorrelatedPtr: public DirectCrossed<_PP, CallCorrelationPtr<typename Info<_PP>::ScalarType>> {};

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
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			m_data = _PP::get();
			assert(isFinite(m_data[0]));
			makeTotalUnit(m_data);
			assert(isFinite(m_data[0]));
		}
	}
	using _PP::changed;

	vector<Scalar> const& get() const { return m_data; }

private:
	vector<Scalar> m_data;
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
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
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

	vector<Scalar> const& get() const { return m_data; }

private:
	vector<Scalar> m_data;
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
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			m_data = _PP::get();
			normalize(m_data);
		}
	}
	using _PP::changed;

	vector<Scalar> const& get() const { return m_data; }

private:
	vector<Scalar> m_data;
};

template <class _PP1, class _PP2>
class Subbed: public _PP1, public _PP2
{
public:
	typedef typename Info<_PP1>::ScalarType Scalar;
	typedef typename Info<_PP1>::ElementType TT;

	void init(EventCompilerImpl* _eci)
	{
		_PP1::init(_eci);
		_PP2::init(_eci);
		m_last = 0;
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP1::execute(_eci, _t, _mag, _phase, _wave);
		_PP2::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP1::changed() || _PP2::changed())
			m_last = _PP1::get() - _PP2::get();
	}
	bool changed() const { return _PP1::changed() || _PP2::changed(); }

	TT const& get() const { return m_last; }

private:
	TT m_last;
};

template <class _PP, unsigned _factorX1000000 = 1000000>
class Scaled: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;
	typedef typename Info<_PP>::ElementType TT;

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_last = 0;
		m_scale = _factorX1000000 / (Scalar)1000000;
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		m_last = _PP::get() * m_scale;
	}
	bool changed() const { return _PP::changed(); }

	TT const& get() const { return m_last; }

private:
	TT m_last;
	Scalar m_scale;
};

template <class _PP>
class Teed
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Teed(_PP* _tee = 0): m_tee(_tee) {}

	Teed& setTeed(_PP& _tee) { m_tee = &_tee; return *this; }

	void init(EventCompilerImpl*) {}
	void execute(EventCompilerImpl*, Time, vector<Scalar> const&, vector<Scalar> const&, std::vector<Scalar> const&) {}
	decltype(_PP().get()) get() const { return m_tee->get(); }
	bool changed() const { return m_tee->changed(); }

private:
	_PP* m_tee;
};

template <class _PP>
class Deltad: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		_PP::init(_eci);
		m_last = 0;
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			m_delta = abs(_PP::get() - m_last);
			m_last = _PP::get();
		}
	}
	using _PP::changed;

	Scalar get() const { return m_delta; }

private:
	Scalar m_last;
	Scalar m_delta;
};

template <class _N = float>
class Centroid
{
public:
	typedef _N Scalar;

	void init(EventCompilerImpl*) {}
	void execute(EventCompilerImpl* _eci, Time, vector<Scalar> const& _mag, vector<Scalar> const&, std::vector<Scalar> const&)
	{
		unsigned b = _eci->bands();
		m_last = 0;
		Scalar total = 0;
		for (unsigned i = 0; i < b; ++i)
			total += sqr(_mag[i]), m_last += sqr(_mag[i]) * i;
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

template <class _PP>
class MATHS: public Historied<_PP>
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci) { _PP::init(_eci); }
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Historied<_PP>::execute(_eci, _t, _mag, _phase, _wave);
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

template <unsigned _HzFrom, unsigned _HzTo, class _N = float>
class BandProfile
{
public:
	typedef _N Scalar;

	void init(EventCompilerImpl* _eci)
	{
		m_from = max<unsigned>(0, _HzFrom / (s_baseRate / _eci->windowSize()));
		m_to = min<unsigned>(_eci->bands(), _HzTo / (s_baseRate / _eci->windowSize()));
		m_last.mean = 0.f;
		m_last.mag = 0.f;
		m_last.sigma = 0.f;
	}
	void execute(EventCompilerImpl*, Time, vector<Scalar> const& _mag, vector<Scalar> const&, vector<Scalar> const&)
	{
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

	GaussianMag<_N> const& get() const { return m_last; }
	bool changed() const { return true; }

private:
	unsigned m_from;
	unsigned m_to;
	GaussianMag<_N> m_last;
};

template <class _PP, unsigned _Size>
class TemporalGaussian: public GenHistoried<_PP, _Size>
{
public:
	typedef GenHistoried<_PP, _Size> Super;
	typedef typename Info<_PP>::ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = zero_of<GenGaussian<typename Super::ElementType> >::value();
	}
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
		if (Super::changed())
		{
			m_last.mean = mean(Super::get());
			m_last.sigma = sigma(Super::get(), m_last.mean);
		}
	}
	GenGaussian<typename Super::ElementType> const& get() const { return m_last; }

private:
	GenGaussian<typename Super::ElementType> m_last;
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

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
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
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
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
	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
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

template <class _PP, unsigned _I>
class Get: public _PP
{
public:
	typedef _PP Super;
	typename remove_const<typename remove_reference<decltype(std::get<_I>(Super().get().toTuple()))>::type>::type get() const
	{
		auto t = Super::get();
		auto tu = t.toTuple();
		return std::get<_I>(tu);
	}
};


static const std::array<float, 26> s_barkBands = {{ 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500, 27000 }};
static const std::array<float, 26> s_barkCentres = {{ 50, 150, 250, 350, 450, 570, 700, 840, 1000, 1170, 1370, 1600, 1850, 2150, 2500, 2900, 3400, 4000, 4800, 5800, 7000, 8500, 10500, 13500, 17500, 22500 }};

template <class _F>
class BandRemapper
{
public:
	template <class _T>
	void init(EventCompilerImpl* _ec, _T const& _bf)
	{
		m_bands.resize(_bf.size());
		for (unsigned i = 0; i < _bf.size(); ++i)
			m_bands[i] = clamp<int, int>(_ec->band(_bf[i]), 0, _ec->bands() - 1);
	}
	std::vector<_F> spectrum(std::vector<_F> const& _mag) const
	{
		std::vector<_F> ret(m_bands.size());
		for (unsigned i = 0; i < 26; ++i)
			ret[i] = mag(_mag, i);
		return ret;
	}
	_F mag(std::vector<_F> const& _mag, unsigned _i) const
	{
		_F ret = 0;
		for (unsigned i = _i ? m_bands[_i - 1] : 0; i < m_bands[_i]; ++i)
			ret += _mag[i];
		return ret;
	}

private:
	std::vector<unsigned> m_bands;
};

static const float s_fr[] = { 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500 };
static const float s_af[] = { 0.532, 0.506, 0.480, 0.455, 0.432, 0.409, 0.387, 0.367, 0.349, 0.330, 0.315, 0.301, 0.288, 0.276, 0.267, 0.259, 0.253, 0.250, 0.246, 0.244, 0.243, 0.243, 0.243, 0.242, 0.242, 0.245, 0.254, 0.271, 0.301 };
static const float s_Lu[] = { -31.6, -27.2, -23.0, -19.1, -15.9, -13.0, -10.3, -8.1, -6.2, -4.5, -3.1, -2.0, -1.1, -0.4, 0.0, 0.3, 0.5, 0.0, -2.7, -4.1, -1.0, 1.7, 2.5, 1.2, -2.1, -7.1, -11.2, -10.7, -3.1 };
static const float s_Tf[] = { 78.5, 68.7, 59.5, 51.1, 44.0, 37.5, 31.5, 26.5, 22.1, 17.9, 14.4, 11.4, 8.6, 6.2, 4.4, 3.0, 2.2, 2.4, 3.5, 1.7, -1.3, -4.2, -6.0, -5.4, -1.5, 6.0, 12.6, 13.9, 12.3 };

template <class T, class U>
static inline T cubicInterpolateIndex(T const* _data, int _len, U _index)
{
	int fi = (int)floor(_index);
	T pi = _index - floor(_index);
	if (pi == 0)
		return _data[fi];
	if (fi >= _len - 1)
		return _data[_len - 1];
	if (fi <= 0)
		return _data[0];

	T m = _data[fi + 1] - _data[fi];
	T mi = fi ? (_data[fi + 1] - _data[fi - 1]) / 2.f : m;
	T mia1 = (fi < _len - 2) ? (_data[fi + 2] - _data[fi]) / 2.f : m;
	T pi2 = sqr(pi);
	T pi3 = cubed(pi);

	return	_data[fi] * (1.f - 3.f * pi2 + 2.f * pi3) + mi * (pi - 2.f * pi2 + pi3) + _data[fi + 1] * (3.f * pi2 - 2.f * pi3) - mia1 * (pi2 - pi3);
}

template <class T>
inline T interpolateValue(T const* _l, unsigned _s, float _v)
{
	if (_v <= _l[0])
		return 0;
	if (_v >= _l[_s - 1])
		return _s - 1;
	T const* i = std::lower_bound(_l, _l + _s, _v);
	if (*i == _v)
		return i - _l;
	assert(*i > _v);
	assert(i != _l);
	return (i - _l) + (_v - *i) / (*i - *(i - 1));
}

template <class _F>
class Phon
{
public:
	template <class _T>
	void init(_T const& _centres)
	{
		m_Tf.resize(_centres.size());
		m_Lu.resize(_centres.size());
		m_af.resize(_centres.size());
		for (unsigned i = 0; i < m_Tf.size(); i++)
		{
			float b = interpolateValue(s_fr, 29, _centres[i]);
			m_Tf[i] = cubicInterpolateIndex(s_Tf, 29, b);
			m_Lu[i] = cubicInterpolateIndex(s_Lu, 29, b);
			m_af[i] = cubicInterpolateIndex(s_af, 29, b);
		}
	}

	_F value(_F _mag, unsigned _i) const
	{
		float db = max(0.f, log10((float)_mag) * 20 + 90);
		return (_F)log10((pow(10.f, (db - 94.f + m_Lu.at(_i)) / 10.f * m_af.at(_i)) - pow(0.4f * pow(10.f, (m_Tf.at(_i) + m_Lu.at(_i)) / 10.f - 9.f), m_af.at(_i))) / 4.47e-3f + 1.15f) / 0.025f;
	}

private:
	std::vector<float> m_Tf;
	std::vector<float> m_Lu;
	std::vector<float> m_af;
};

template <unsigned _From = 0, unsigned _To = 26, class _ScalarType = float>
class BarkPhonSum
{
public:
	typedef _ScalarType ElementType;
	typedef _ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		m_bark.init(_eci, s_barkBands);
		m_phon.init(s_barkCentres);
		m_last = 0;
	}
	void execute(EventCompilerImpl*, Time, std::vector<Scalar> const& _mag, std::vector<Scalar> const&, std::vector<Scalar> const&)
	{
		m_last = 0;
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last += m_phon.value(m_bark.mag(_mag, cb), cb);
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	BandRemapper<Scalar> m_bark;
	Phon<Scalar> m_phon;
};

template <unsigned _From = 0, unsigned _To = 26, class _ScalarType = float>
class BarkPhon
{
public:
	typedef _ScalarType Scalar;
	typedef array<Scalar, _To - _From> ElementType;

	void init(EventCompilerImpl* _eci)
	{
		m_bark.init(_eci, s_barkBands);
		m_phon.init(s_barkCentres);
		m_last.fill(0);
	}
	void execute(EventCompilerImpl*, Time, std::vector<Scalar> const& _mag, std::vector<Scalar> const&, std::vector<Scalar> const&)
	{
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last[cb - _From] = m_phon.value(m_bark.mag(_mag, cb), cb);
	}
	ElementType const& get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	BandRemapper<Scalar> m_bark;
	Phon<Scalar> m_phon;
};

template <unsigned _From, unsigned _To, class _ScalarType>
class GenSum<BarkPhon<_From, _To, _ScalarType>>: public BarkPhon<_From, _To, _ScalarType>
{
public:
	typedef BarkPhon<_From, _To, _ScalarType> Super;
	typedef _ScalarType ElementType;
	typedef _ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = 0;
		m_scale = 1 / Scalar(_To - _From) / 100;
	}
	void execute(EventCompilerImpl* _eci, Time _t, std::vector<Scalar> const& _mag, std::vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
		auto const& x = Super::get();
		m_last = 0;
		for (unsigned cb = _From; cb < _To; ++cb)
			m_last += x[cb];
		m_last *= m_scale;
		m_last -= 0.5;
	}
	ElementType get() const { return m_last; }
	bool changed() const { return true; }

private:
	ElementType m_last;
	Scalar m_scale;
};

template <class _PP>
class DiffSum: public _PP
{
};

template <unsigned _From, unsigned _To, class _ScalarType>
class DiffSum<BarkPhon<_From, _To, _ScalarType>>: public BarkPhon<_From, _To, _ScalarType>
{
public:
	typedef BarkPhon<_From, _To, _ScalarType> Super;
	typedef _ScalarType ElementType;
	typedef _ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		Super::init(_eci);
		m_last = 0;
		m_in.fill(0);
	}
	void execute(EventCompilerImpl* _eci, Time _t, std::vector<Scalar> const& _mag, std::vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
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
	void execute(EventCompilerImpl* _eci, Time _t, std::vector<Scalar> const& _mag, std::vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		Super::execute(_eci, _t, _mag, _phase, _wave);
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

template <unsigned _FromHz = 0, unsigned _ToHz = 96000, class _ScalarType = float>
class Power
{
public:
	typedef _ScalarType ElementType;
	typedef _ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		m_last = 0;
		m_from = _eci->band(_FromHz);
		m_to = _eci->band(_ToHz);
	}
	void execute(EventCompilerImpl*, Time, std::vector<Scalar> const& _mag, std::vector<Scalar> const&, std::vector<Scalar> const&)
	{
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

template <unsigned _FromHz = 0, unsigned _ToHz = 96000, class _ScalarType = float>
class DeltaPower: public Deltad<Power<_FromHz, _ToHz, _ScalarType>> {};

/*
// Chord
{
	array<float, 12> chord;
	chord.fill(0);
	float tmaga = 0;
	float tmagb = 0;
	float tsim = 0;
	for (unsigned t = 0; t < 12; ++t)
	{
		// calculate mt, the maximum i that is in range [_f, _t) for the tone t.
		for (unsigned o = 0; o < 8; ++o)
		{
			unsigned fi = max<int>(_f, (int)round(band(toneFrequency(t - .5f, (float)o))));
			unsigned ti = min<int>(_t, (int)round(band(toneFrequency(t + .5f, (float)o))));
			if (fi < _t || ti >= _f)
				for (unsigned i = fi; i < ti; ++i)
					chord[o] += sqr((1.f - fabs(mod1(_phase[i] - m_phase[i] + 1) - mod1(m_phase[i] - m_lphase[i] + 1))) * _mag[i]);
		}

		tsim += m_lastChord[_b][t] * chord[t];
		tmaga += sqr(m_lastChord[_b][t]);
		tmagb += sqr(chord[t]);
	}
	float tot = 0;
	for (unsigned t = 0; t < 12; ++t)
	{
		m_lastChord[_b][t] = max(0.00000001f, m_lastChord[_b][t] * chord[t]);
		tot += m_lastChord[_b][t];
	}
	float scale = 1.f / tot;
	for (unsigned t = 0; t < 12; ++t)
		m_lastChord[_b][t] *= scale;

	return tsim / (sqrt(tmaga) * sqrt(tmagb));
}
*/
template <unsigned _FromHz = 0, unsigned _ToHz = 96000, class _ScalarType = float>
class BandwiseMagIncrease
{
	typedef _ScalarType ElementType;
	typedef _ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		m_last = 0;
		m_from = _eci->band(_FromHz);
		m_to = _eci->band(_ToHz);
		m_lastIn.clear();
	}
	void execute(EventCompilerImpl*, Time _t, std::vector<Scalar> const& _mag, std::vector<Scalar> const&, std::vector<Scalar> const&)
	{
		m_last = 0;
		auto* li = m_lastIn.data();
		if (m_lastIn.size())
			for (unsigned b = m_from; b < m_to; ++b, ++li)
			{
				m_last += sqr(max<Scalar>(0, _mag[b] - *li));
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

template <unsigned _FromHz = 0, unsigned _ToHz = 96000, class _ScalarType = float, bool _Invert = false>
class TonalPower
{
public:
	typedef _ScalarType ElementType;
	typedef _ScalarType Scalar;

	void init(EventCompilerImpl* _eci)
	{
		m_last = 0;
		m_from = _eci->band(_FromHz);
		m_to = _eci->band(_ToHz);
		m_lphase.clear();
		m_phase.clear();
	}
	void execute(EventCompilerImpl*, Time, std::vector<Scalar> const& _mag, std::vector<Scalar> const& _phase, std::vector<Scalar> const&)
	{
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

template <unsigned _FromHz = 0, unsigned _ToHz = 96000, class _ScalarType = float>
class NonTonalPower: public TonalPower<_FromHz, _ToHz, _ScalarType, true> {};

}
