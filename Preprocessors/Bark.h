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
#include "Arithmetical.h"

namespace lb
{

static const std::array<float, 26> s_barkBands = {{ 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500, 27000 }};
static const std::array<float, 26> s_barkCentres = {{ 50, 150, 250, 350, 450, 570, 700, 840, 1000, 1170, 1370, 1600, 1850, 2150, 2500, 2900, 3400, 4000, 4800, 5800, 7000, 8500, 10500, 13500, 17500, 22500 }};

template <class _F>
class BandRemapper
{
public:
	template <class _T>
	void init(unsigned _bandCount, float _windowSeconds, _T const& _bf)
	{
		m_bands.resize(_bf.size());
		for (unsigned i = 0; i < _bf.size(); ++i)
			m_bands[i] = band(_bandCount, _windowSeconds, _bf[i]);
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
		float db = std::max<float>(0.f, log10((float)_mag) * 20 + 90);
		return (_F)log10((pow(10.f, (db - 94.f + m_Lu.at(_i)) / 10.f * m_af.at(_i)) - pow(0.4f * pow(10.f, (m_Tf.at(_i) + m_Lu.at(_i)) / 10.f - 9.f), m_af.at(_i))) / 4.47e-3f + 1.15f) / 0.025f;
	}

private:
	std::vector<float> m_Tf;
	std::vector<float> m_Lu;
	std::vector<float> m_af;
};

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

template <class _PP, unsigned _From, unsigned _To>
class GenSum<BarkPhon<_PP, _From, _To>>: public BarkPhon<_PP, _From, _To>
{
public:
	typedef BarkPhon<_PP, _From, _To> Super;
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
class DiffSum<BarkPhon<_PP, _From, _To>>: public BarkPhon<_PP, _From, _To>
{
public:
	typedef BarkPhon<_PP, _From, _To> Super;
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
