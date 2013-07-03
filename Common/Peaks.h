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
#include <map>

#include "Global.h"
#include "Maths.h"

namespace lb
{

std::vector<float> const& parabolaAC(unsigned _s, float _period);
float parabolaACf(float _x, float _period, unsigned _lobe = 0);

template <class T>
std::pair<T, T> parabolicPeakOf(T _a, T _b, T _c)
{
	T p = (_a - _c) / (2 * (_a - 2 * _b + _c));
	return std::make_pair(p, _b - (_a - _c) * p / 4.f);
}

std::map<float, float> parabolicPeaks(std::vector<float> const& _s);
std::map<float, float> parabolicPeaks(float const* _s, unsigned _n);

template <class _T> std::map<typename element_of<_T>::type, unsigned> peaks(_T const& _s)
{
	using E = typename element_of<_T>::type;
	std::map<E, unsigned> ret;
	int start = -1;
	int peak = -1;
	for (unsigned i = 1; i < _s.size(); ++i)
	{
		if (_s[i] < _s[i - 1]) // descending
		{
			if (start != -1 && peak == -1) // were climbing
				peak = i - 1;
		}
		if (_s[i] > _s[i - 1] || i == _s.size() - 1)	// climbing
		{
			if (start != -1 && peak != -1)
			{
				// found a peak
//				ret[_s[peak] - min(_s[start], _s[i - 1])] = peak;
				ret[_s[peak]] = peak;
				start = -1;
				peak = -1;
			}
			if (start == -1)
				start = i - 1;
		}
	}
	return ret;
}

template <class _T = float> LIGHTBOX_STRUCT(2, Peak, _T, band, _T, mag);

template <class _T>
unsigned topPeaks(_T const& _s, foreign_vector< Peak<typename element_of<_T>::type> > _top)
{
	using P = Peak<typename element_of<_T>::type>;
	for (auto& i: _top)
		i = P(-1, -1);
	auto n = _s.size();
	int start = -1;
	int peak = -1;
	unsigned ret = 0;
	for (unsigned i = 1; i < n; ++i)
	{
		if (_s[i] < _s[i - 1]) // descending
		{
			if (start != -1 && peak == -1) // were climbing
				peak = i - 1;
		}
		if (_s[i] > _s[i - 1] || i == n - 1)	// climbing
		{
			if (start != -1 && peak != -1)
			{
				// found a peak
				auto pk = parabolicPeakOf(_s[peak - 1], _s[peak], _s[peak + 1]);
				// get local-relative strength +/- 5% of the width.
				P current(pk.first + peak, pk.second - (_s[start] + _s[i - 1]) / 2);

				for (size_t ins = 0; ins <= _top.size(); ++ins)
					// If we've reached the end, no comparison necessary; otherwise finish if the item at the candidate insert position is >= us (and not empty <- -1).
					if (ins == _top.size() || (_top[ins].band != -1 && current.mag < _top[ins].mag))
					{
						// If the insert position is in the list, record. Then exit.
						if (ins > 0)
						{
							_top[ins - 1] = current;
							++ret;
						}
						break;
					}
					else if (ins + 1 < _top.size())
						// We passed an item - move the item above them into their place.
						_top[ins] = _top[ins + 1];

				start = -1;
				peak = -1;
			}
			if (start == -1)
				start = i - 1;
		}
	}
	return std::min(_top.size(), ret);
}

}
