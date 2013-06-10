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

template <class _PP>
class Teed
{
public:
	typedef typename Info<_PP>::ScalarType Scalar;

	Teed(_PP* _tee = 0): m_tee(_tee) {}

	Teed& setTeed(_PP& _tee) { m_tee = &_tee; return *this; }

	void init(EventCompilerImpl*) {}
	void execute(EventCompilerImpl*, std::vector<Scalar> const&) {}
	decltype(_PP().get()) get() const { return m_tee->get(); }
	bool changed() const { return m_tee->changed(); }

private:
	_PP* m_tee;
};

template <class _PP, unsigned _I>
class Get: public _PP
{
public:
	typedef _PP Super;
	typename std::remove_const<typename std::remove_reference<decltype(std::get<_I>(Super().get().toTuple()))>::type>::type get() const
	{
		auto t = Super::get();
		auto tu = t.toTuple();
		return std::get<_I>(tu);
	}
};

}
