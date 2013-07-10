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

#include <type_traits>
#include <cmath>
#include <vector>
#include <map>
#include <boost/math/special_functions/fpclassify.hpp>

#include "Global.h"
#include "Statistics.h"
#include "UnitTesting.h"

namespace lb
{

static const float MinusInfinity = -std::numeric_limits<float>::infinity();

template <class _T> inline _T dbFromPower(_T _l) { return _l > 0 ? log10(_l) * 10 : -255; }
template <class _T> inline _T dbFromAmp(_T _l) { return _l > 0 ? log10(_l) * 20 : -255; }

/// Linear interpolate with templated fixed-point (thousandths) factor.
template <int _x> inline float lerp(float _a, float _b) { return _a * (1.f - _x / 1000.f) + _b * _x / 1000.f; }

template <class _T> inline _T lerp(double _x, _T _a, _T _b) { return _a + (_b - _a) * _x; }

template <class _T> double lext(_T _v, _T _a, _T _b) { return double(_v - _a) / (_b - _a); }

template <class _T> inline _T lext(_T _a, _T _b)
{
	return _b / (_a + _b);
}

template <class _T> inline _T llerp(_T _x, _T _a, _T _b)
{
	return pow(_a, 1.f - _x) * pow(_b, _x);
}

inline float mod1(float _f)
{
	float d;
	return modff(_f, &d);
}

template <class _T>
typename std::remove_reference<decltype(_T()[0])>::type sumOf(_T const& _r)
{
	typedef typename std::remove_reference<decltype(_T()[0])>::type R;
	R ret = R(0);
	foreach (auto i, _r)
		ret += i;
	return ret;
}

template <class _T>
typename std::remove_reference<decltype(_T()[0])>::type productOf(_T const& _r)
{
	typedef typename std::remove_reference<decltype(_T()[0])>::type R;
	R ret = R(1);
	foreach (auto i, _r)
		ret *= i;
	return ret;
}

template <class _T>
typename std::remove_reference<decltype(_T()[0])>::type magnitudeOf(_T const& _r)
{
	typedef typename std::remove_reference<decltype(_T()[0])>::type R;
	R ret = R(0);
	for (auto i: _r)
		ret += sqr(i);
	return sqrt(ret);
}

template <class _Ta, class _Tb>
typename std::remove_reference<decltype(_Ta()[0] * _Tb()[0])>::type cosineSimilarity(_Ta const& _a, _Tb const& _b)
{
	assert(_a.size() == _b.size());
	typedef typename std::remove_reference<decltype(_Ta()[0] * _Tb()[0])>::type R;
	float ma = magnitudeOf(_a);
	float mb = magnitudeOf(_b);
	if (ma == 0.f || mb == 0.f)
		return 0.f;
	R ret = R(0);
	auto bii = _b.begin();
	for (auto ai: _a)
		ret += ai * *(bii++);
	return ret /= (ma * mb);
}

template <class _T>
bool similar(_T _a, _T _b, double _ratio = 0.05)
{
	return _a == _b ? true : (std::max(_a, _b) - std::min(_a, _b)) / double(fabs(_a) + fabs(_b)) < _ratio / 2;
}

template <class _T>
double dissimilarity(_T _a, _T _b)
{
	return (std::max(_a, _b) - std::min(_a, _b)) / double(_a + _b) * 2;
}

template <class _T, class _U>
typename _T::iterator nearest(_T& _map, _U const& _v)
{
	auto it1 = _map.lower_bound(_v);
	auto it2 = it1;
	if (it2 != _map.begin())
		it2--;
	return (it1 == _map.end() || fabs(it2->first - _v) < fabs(it1->first - _v)) ? it2 : it1;
}

template <class _T>
void normalize(_T& _v)
{
	auto r = range(_v);
	auto d = r.second - r.first;
	if (d > 0)
	{
		auto s = 1 / d;
		for (auto& v: _v)
			v = (v - r.first) * s;
	}
}

template <class _T>
decltype(_T()[0]) makeUnitMagnitude(_T& _v)
{
	auto m = magnitudeOf(_v);
	if (m)
	{
		auto s = 1 / m;
		for (auto& i: _v)
			i *= s;
	}
}

template <class _T>
void makeUnitSum(_T& _v)
{
	auto total = _v[0] - _v[0];
	for (auto const& n: _v)
		total += n;
	if (total > 0)
	{
		auto scale = 1 / total;
		for (auto& p: _v)
			p *= scale;
	}
	else
	{
		total = 1;
		total /= _v.size();
		for (auto& p: _v)
			p = total;
	}
}

inline double fracPart(double _f) { double r; return modf(_f, &r); }

/// Is the number finite?
inline bool isFinite(float _x)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
//	return _x == _x && !isinf(_x);
	return (reinterpret_cast<uint32_t&>(_x) & 0x7fffffff) < 0x7f800000;
#pragma GCC diagnostic pop
}

template <class T, class U> inline T clamp(T _v, U _min, U _max)
{
	return _v < (T)_min ? (T)_min : _v > (T)_max ? (T)_max : isFinite(_v) ? _v : (T)_min;
}

template <class T, class U> inline T clamp(T _v, std::pair<U, U> const& _minMax)
{
	return _v < (T)_minMax.first ? (T)_minMax.first : _v < (T)_minMax.second ? _v : (T)_minMax.second;
}

template <class T> inline T clamp(T _v)
{
	return _v < (T)0 ? (T)0 : _v < (T)1 ? _v : (T)1;
}

inline double abs(double _v) { return ::fabs(_v); }
inline float abs(float _v) { return ::fabs(_v); }
template <class _T> inline _T abs(_T _v) { return ::abs(_v); }

template <class _T> _T cubed(_T _x) { return _x * _x * _x; }

std::vector<float> solveQuadratic(float a, float b, float c);
std::vector<float> solveCubic(float a, float b, float c, float d);
inline float cubicBezierT(float _t, float _z)
{
	return 3 * _z * sqr(1 - _t) * _t + 3 * (1 - _t)*_t*_t + _t*_t*_t;
}

/// Bias an x == y curve by some amount _z.
float bias(float _x, float _z);

/// Power scale helper.
float powScale(float _x, float _z);

/// Get the "io" of a number.
template <class T> inline T io(T x)
{
	T const t = 1.e-08;
	T y = 0.5 * x;
	T e = 1.0;
	T de = 1.0;
	T xi;
	T sde;
	for (int i = 1; i < 26; ++i)
	{
		xi = T(i);
		de *= y / xi;
		sde = de * de;
		e += sde;
		if (e * t - sde > 0)
			break;
	}
	return e;
}

/// Different window functions supported.
LIGHTBOX_TEXTUAL_ENUM(WindowFunction, RectangularWindow, HannWindow, HammingWindow, TukeyWindow, KaiserWindow, BlackmanWindow, GaussianWindow)

/// Build a window function as a vector.
std::vector<float> windowFunction(unsigned _size, WindowFunction _f, float _parameter = 0.5f);

/// Transform a window function into its zero-phase variant (i.e. rotate by half its width).
std::vector<float> zeroPhaseWindow(std::vector<float> const& _w);

/// Returns a window that is the given window element-wise scaled by the factor _f.
std::vector<float> scaledWindow(std::vector<float> const& _w, float _f);

inline float toneFrequency(float _t) { return 27.5f * pow(2, (_t - 9) / 12.f); }
inline float toneFrequency(float _t, float _o) { return toneFrequency(_t + _o * 12); }
float toneFrequency(unsigned _t, unsigned _o);

LIGHTBOX_UNITTEST(7, "toneFrequency")
{
	requireEqual(toneFrequency(9u, 4u), 440.f, "Multiples population");
	requireApproximates(toneFrequency(0u, 0u), 16.35f, "Octave population");
	requireApproximates(toneFrequency(10u, 7u), 3729.31f, "General population");
}

static const std::array<float, 26> s_barkBands = {{ 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500, 27000 }};
static const std::array<float, 26> s_barkCentres = {{ 50, 150, 250, 350, 450, 570, 700, 840, 1000, 1170, 1370, 1600, 1850, 2150, 2500, 2900, 3400, 4000, 4800, 5800, 7000, 8500, 10500, 13500, 17500, 22500 }};

inline float band(unsigned _bandCount, float _windowSeconds, float _frequency) { return clamp<float>(_frequency * _windowSeconds, 0.f, _bandCount - 1.f); }

template <class _T>
struct first_element
{
	using type = typename std::remove_const<typename std::remove_reference<decltype(_T()[0])>::type>::type;
};

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
	template <class _T> typename first_element<_T>::type spectrum(_T const& _mag) const
	{
		typename first_element<_T>::type ret(m_bands.size());
		for (unsigned i = 0; i < 26; ++i)
			ret[i] = mag(_mag, i);
		return ret;
	}
	template <class _T> typename first_element<_T>::type mag(_T const& _mag, unsigned _i) const
	{
		typename first_element<_T>::type ret = 0;
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

}
