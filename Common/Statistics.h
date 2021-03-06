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

#include <cmath>
#include <vector>
#include <array>
#include <map>

#include "Global.h"

namespace lb
{

template <class _T> struct zero_of { static _T value() { return _T(0); } };
template <class _T, size_t _Z> struct zero_of<std::array<_T, _Z>> { static std::array<_T, _Z> value() { std::array<_T, _Z> r; r.fill(0); return r; } };

/// Square a number.
template <class T> inline T sqr(T _t) { return _t * _t; }

/// Sign of a number
template <class T> inline T sign(T _t) { return _t ? _t > 0 ? 1 : -1 : 0; }

/// Get range of numeric collection.
template <class T>
inline std::pair<typename element_of<T>::type, typename element_of<T>::type> range(T const& _t)
{
	if (_t.begin() == _t.end())
		return std::make_pair(0, 1);
	auto ret = std::make_pair(*_t.begin(), *_t.begin());
	for (auto i = _t.begin(); i != _t.end(); ++i)
	{
		ret.first = std::min(ret.first, *i);
		ret.second = std::max(ret.second, *i);
	}
	return ret;
}

template <class T>
inline std::pair<T, T> range(T const* _begin, T const* _end)
{
	auto ret = _begin == _end ? std::pair<T, T>(0, 1) : std::make_pair(*_begin, *_begin);
	for (auto i = _begin; i != _end; ++i)
	{
		ret.first = std::min(ret.first, *i);
		ret.second = std::max(ret.second, *i);
	}
	return ret;
}

template <class T>
typename element_of<T>::type mean(T const& _distro)
{
	typename element_of<T>::type ret = zero_of<typename element_of<T>::type>::value();
	if (!_distro.size())
		return ret;
	for (auto v: _distro)
		ret += v;
	return ret / _distro.size();
}

template <class T>
typename element_of<T>::type mean(T const& _begin, T const& _end)
{
	typename element_of<T>::type ret = zero_of<typename element_of<T>::type>::value();
	unsigned j = 0;
	for (T i = _begin; i != _end; ++i, ++j)
		ret += *i;
	if (!j)
		return ret;
	return ret / j;
}

template <class T>
typename element_of<T>::type variance(T const& _distro, typename element_of<T>::type _mean)
{
	typename element_of<T>::type ret = zero_of<typename element_of<T>::type>::value();
	if (!_distro.size())
		return ret;
	for (auto v: _distro)
		ret += sqr(v - _mean);
	return ret / _distro.size();
}

template <class T>
typename element_of<T>::type variance(T const& _begin, T const& _end, typename element_of<T>::type _mean)
{
	typename element_of<T>::type ret = zero_of<typename element_of<T>::type>::value();
	unsigned j = 0;
	for (T i = _begin; i != _end; ++i, ++j)
		ret += sqr(*i - _mean);
	if (!j)
		return ret;
	return ret / j;
}

template <class T>
typename element_of<T>::type sigma(T const& _distro, typename element_of<T>::type _mean)
{
	return sqrt(variance(_distro, _mean));
}

template <class T>
typename element_of<T>::type sigma(T const& _begin, T const& _end, typename element_of<T>::type _mean)
{
	return sqrt(variance(_begin, _end, _mean));
}

template <class T>
typename element_of<T>::type variance(T const& _distro)
{
	return variance(_distro, mean(_distro));
}

template <class T>
typename element_of<T>::type variance(T const& _begin, T const& _end)
{
	return variance(_begin, _end, mean(_begin, _end));
}

template <class T>
typename element_of<T>::type sigma(T const& _distro)
{
	return sigma(_distro, mean(_distro));
}

template <class T>
typename element_of<T>::type sigma(T const& _begin, T const& _end)
{
	return sigma(_begin, _end, mean(_begin, _end));
}

template <class T>
T normal(T _x, T _mu, T _sigma)
{
	if (!_sigma)
		return 0;
	T ep = -(sqr(_x - _mu) / (2 * sqr(_sigma)));
	T pb = sqrt(twoPi<T>() * _sigma);
	return exp(ep) / pb;
}

template <class T>
T nnormal(T _x, T _mu, T _sigma)
{
	return normal(_x, _mu, _sigma) * sqrt(_sigma);
}

template <class T>
T nnormal(T _x, std::tuple<T, T> const& _muSigma)
{
	return normal(_x, _muSigma.first, _muSigma.second) * sqrt(_muSigma.first);
}

LIGHTBOX_STRUCT(2, Gaussian, double, mean, double, sigma);

template <class _T> LIGHTBOX_STRUCT(2, GenGaussian, _T, mean, _T, sigma);

template <class _F> struct zero_of<GenGaussian<_F> > { static GenGaussian<_F> value() { return GenGaussian<_F>(zero_of<_F>::value(), zero_of<_F>::value()); } };

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

template <class T>
T normal(T _x, Gaussian _muSigma, T _rootScale = 1)
{
	return normal<T>(_x, _muSigma.mean, _muSigma.sigma) * _rootScale;
}

template <class T>
T nnormal(T _x, Gaussian _muSigma)
{
	return normal<T>(_x, _muSigma.mean, _muSigma.sigma) * sqrt(_muSigma.sigma);
}

template <class T>
typename element_of<T>::type pInDistro(typename element_of<T>::type _v, T const& _distro, typename element_of<T>::type _backupSigma = 0)
{
	if (!_distro.size())
		return 0;
	typedef typename element_of<T>::type E;
	E m = mean(_distro);
	E s = _distro.size() > 1 ? sigma(_distro, m) : _backupSigma ? _backupSigma : m;
	if (s == 0)
		return (m == _v) ? 1 : 0;
	s *= E(_distro.size() + 1) / _distro.size();
//	cerr << "pInDistro(" << _v << ", " << _distro << ") -> " << m << ", " << v << " => " << normal(_v, m, v) << endl;
	return normal(_v, m, s);
}

template <class T>
typename element_of<T>::type pivotalBadness(T const& _distro, typename T::iterator _pivot, typename element_of<T>::type  _minSigma = 0)
{
	return std::max(_minSigma, sigma(_distro.begin(), _pivot)) + std::max(_minSigma, sigma(_pivot, _distro.end()));
}

template <class T>
typename element_of<T>::type pivotalBadness(T const& _distro, typename element_of<T>::type _v, typename element_of<T>::type _minSigma = 0)
{
	return pivotalBadness(_distro, std::find(_distro.begin(), _distro.end(), _v, _minSigma));
}

template <class T>
typename T::iterator pivot(T const& _distro, unsigned _minPartition = 1, typename element_of<T>::type _minSigma = 0)
{
	// assumes _distro is sorted.
	if (!is_sorted(_distro.begin(), _distro.end()))
		return _distro.end();
	if (_distro.size() < _minPartition * 2)
		return _distro.end();

//	cout << "   pivot: " << _distro << ": ";

	typedef typename element_of<T>::type E;
	auto plower = --next(_distro.begin(), _minPartition);
	auto pupper = ++next(_distro.begin(), _distro.size() - _minPartition);
	auto p = next(_distro.begin(), _distro.size() / 2);

	E badness = pivotalBadness(_distro, p, _minSigma);
	E b;

	auto pp = p;
	E pb = badness;
	for (++pp; pp != pupper && (b = pivotalBadness(_distro, pp, _minSigma)) <= pb; ++pp)
		pb = b;
	--pp;

	auto pm = p;
	E mb = badness;
	for (--pm; pm != plower && (b = pivotalBadness(_distro, pm, _minSigma)) <= mb; --pm)
		mb = b;
	++pm;

	p = (mb < pb) ? pm : pp;

//	cout << "best @ " << *pm << "/" << *pp << endl;
	return p;
}

}
