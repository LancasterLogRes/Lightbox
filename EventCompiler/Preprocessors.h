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
struct Float
{
	constexpr static float value = Float<_Base, _Exp + 1>::value / 10.f;
};

template <int _Base>
struct Float<_Base, 0>
{
	static constexpr float value = _Base;
};

template <class _E> struct get_scalar { typedef _E type; };
template <class _E> struct get_scalar<vector<_E>> { typedef typename get_scalar<_E>::type type; };
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

template <class _PP, unsigned _df = 1u>
class Downsampled: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Downsampled(unsigned _f = _df): m_factor(_f), m_acc(0), m_count(_f) {}

	Downsampled& setDownsample(unsigned _f) { m_factor = _f; m_acc = 0; m_count = _f; return *this; }

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
			m_acc += _PP::get();
			++m_count;
		}
	}

	Scalar get() const { return m_acc / m_count; }
	bool changed() const { return m_count == m_factor; }

private:
	unsigned m_factor;
	Scalar m_acc;
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
		cdebug << "Historied::init";
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

template <class _PP, unsigned _ds = 8>
class SlowHistoried: public _PP
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	SlowHistoried(unsigned _s = _ds): m_data(_s) {}

	SlowHistoried& setHistory(unsigned _s) { m_data.clear(); m_data.resize(_s + 4, 0); m_offset = 0; return *this; }

	void init(EventCompilerImpl* _eci)
	{
		cdebug << "Historied::init";
		_PP::init(_eci);
		setHistory(m_data.size() - 4);
	}

	void resetBefore(unsigned _bins) {}

	void execute(EventCompilerImpl* _eci, Time _t, vector<Scalar> const& _mag, vector<Scalar> const& _phase, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _t, _mag, _phase, _wave);
		if (_PP::changed())
		{
			m_data[m_data.size() - 4 + m_offset] = _PP::get();
			++m_offset;
			if (m_offset == 4)
			{
				memmove(m_data.data(), m_data.data() + 4, (m_data.size() - 4) * sizeof(Scalar));
				m_offset = 0;
			}
		}
	}

	bool changed() const { return _PP::changed(); }
	foreign_vector<Scalar> get() const { return foreign_vector<Scalar>(const_cast<Scalar*>(m_data.data()) + m_offset, m_data.size() - 4); }

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
			memmove(m_data.data(), m_data.data() + 1, (m_data.size() - 1) * sizeof(ElementType));
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
template <class _N = float> struct CallCorrelation { inline static _N call(typename vector<_N>::const_iterator _a, typename vector<_N>::const_iterator _b, unsigned _s) { return correlation(_a, _b, _s); } };
template <class _N = float> struct CallDissimilarity { inline static _N call(typename vector<_N>::const_iterator _a, typename vector<_N>::const_iterator _b, unsigned _s) { return dissimilarity(_a, _b, _s); } };

/*
template <class _PP> using Correlated = Crossed<_PP, CallCorrelation>;
template <class _PP> using Similarity = Crossed<_PP, CallSimilarity>;
template <class _PP> using Dissimilarity = Crossed<_PP, CallDissimilarity>;
*/
template <class _PP> class Correlated: public Crossed<_PP, CallCorrelation<typename Info<_PP>::ScalarType>> {};
template <class _PP> class Similarity: public Crossed<_PP, CallSimilarity<typename Info<_PP>::ScalarType>> {};
template <class _PP> class Dissimilarity: public Crossed<_PP, CallDissimilarity<typename Info<_PP>::ScalarType>> {};

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
			m_delta = fabs(_PP::get() - m_last);
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

template <class _PP, class _TroughRatio = Float<1, 0> >
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

}
