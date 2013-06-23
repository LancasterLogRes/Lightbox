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
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
		if (_PP::changed())
		{
			auto const& h = _PP::get();
			assert(isFinite(h[0]));
			unsigned s = h.size();
			if (m_lastAc.empty())
				m_lastAc.resize((s - 1) / 2, 0);
			autocross(h.begin(), s, _X::call, (s - 1) / 2, 1, m_lastAc);
//			m_lastAc = autocross(h.begin(), s, _X::call, s / 2);
			assert(!m_lastAc.size() || isFinite(m_lastAc[0]));
		}
	}

	std::vector<Scalar> const& get() const { return m_lastAc; }
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
	std::vector<Scalar> m_lastAc;
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
	void execute(EventCompilerImpl* _eci, std::vector<Scalar> const& _wave)
	{
		_PP::execute(_eci, _wave);
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

	std::vector<Scalar> const& get() const { return m_lastAc; }
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
	std::vector<Scalar> m_lastAc;
};

template <class _N = float> struct CallSimilarity { inline static _N call(typename std::vector<_N>::const_iterator _a, typename std::vector<_N>::const_iterator _b, unsigned _s) { return similarity(_a, _b, _s); } };
template <class _N = float> struct CallCorrelation { inline static _N call(typename std::vector<_N>::const_iterator _a, typename std::vector<_N>::const_iterator _b, unsigned _s) { return correlation<_N>(_a, _b, _s); } };
template <class _N = float> struct CallDissimilarity { inline static _N call(typename std::vector<_N>::const_iterator _a, typename std::vector<_N>::const_iterator _b, unsigned _s) { return dissimilarity(_a, _b, _s); } };
template <class _N = float> struct CallCorrelationPtr { inline static _N call(_N const* _a, _N const* _b, unsigned _s) { return correlation(_a, _b, _s); } };
template <class _N = float> struct CallSimilarityPtr { inline static _N call(_N const* _a, _N const* _b, unsigned _s) { return similarity(_a, _b, _s); } };

/*
template <class _PP> using Correlated = Crossed<_PP, CallCorrelation>;
template <class _PP> using Similarity = Crossed<_PP, CallSimilarity>;
template <class _PP> using Dissimilarity = Crossed<_PP, CallDissimilarity>;
*/
template <class _PP> class Correlated: public Crossed<_PP, CallCorrelation<typename Info<_PP>::ScalarType>> {};
template <class _PP> class Similarity: public Crossed<_PP, CallSimilarity<typename Info<_PP>::ScalarType>> {};
template <class _PP> class Dissimilarity: public Crossed<_PP, CallDissimilarity<typename Info<_PP>::ScalarType>> {};
template <class _PP> class CorrelatedPtr: public DirectCrossed<_PP, CallCorrelationPtr<typename Info<_PP>::ScalarType>> {};
template <class _PP> class SimilarityPtr: public DirectCrossed<_PP, CallSimilarityPtr<typename Info<_PP>::ScalarType>> {};

}
