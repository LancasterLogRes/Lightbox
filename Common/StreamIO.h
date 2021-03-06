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

#include <array>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include <tuple>
#include <sstream>
#include <iostream>

#include "Global.h"
#include "Time.h"

#undef _S
#undef _T

namespace lb
{

template <class T> std::string toString(T const& _t)
{
	std::stringstream ss;
	ss << _t;
	return ss.str();
}

std::string toString(double _d);
std::string toString(float _f);
inline std::string toString(std::string const& _s) { return "\"" + _s + "\""; }

template <class T> void fromString(T& _t, std::string const& _s)
{
	std::istringstream ss(_s);
	ss >> _t;
}
void fromString(std::string& _t, std::string const& _s);

template <class S, class T> struct StreamOut { static S& bypass(S& _out, T const& _t) { _out << _t; return _out; } };
template <class S> struct StreamOut<S, uint8_t> { static S& bypass(S& _out, uint8_t const& _t) { _out << (int)_t; return _out; } };

template <class S, class T>
inline S& streamout(S& _out, std::vector<T> const& _e)
{
	_out << "[";
	if (!_e.empty())
	{
		StreamOut<S, T>::bypass(_out, _e.front());
		for (auto i = ++_e.begin(); i != _e.end(); ++i)
			StreamOut<S, T>::bypass(_out << ",", *i);
	}
	_out << "]";
	return _out;
}

template <class T> inline std::ostream& operator<<(std::ostream& _out, std::vector<T> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T, unsigned Z>
inline S& streamout(S& _out, std::array<T, Z> const& _e)
{
	_out << "[";
	if (!_e.empty())
	{
		StreamOut<S, T>::bypass(_out, _e.front());
		auto i = _e.begin();
		for (++i; i != _e.end(); ++i)
			StreamOut<S, T>::bypass(_out << ",", *i);
	}
	_out << "]";
	return _out;
}
template <class T, unsigned Z> inline std::ostream& operator<<(std::ostream& _out, std::array<T, Z> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T, unsigned long Z>
inline S& streamout(S& _out, std::array<T, Z> const& _e)
{
	_out << "[";
	if (!_e.empty())
	{
		StreamOut<S, T>::bypass(_out, _e.front());
		auto i = _e.begin();
		for (++i; i != _e.end(); ++i)
			StreamOut<S, T>::bypass(_out << ",", *i);
	}
	_out << "]";
	return _out;
}
template <class T, unsigned long Z> inline std::ostream& operator<<(std::ostream& _out, std::array<T, Z> const& _e) { streamout(_out, _e); return _out; }

inline std::ostream& operator<<(std::ostream&& _out, Time const& _n) { _out << textualTime(_n); return _out; }

template <class S, class T>
inline S& streamout(S& _out, std::list<T> const& _e)
{
	_out << "[";
	if (!_e.empty())
	{
		_out << _e.front();
		for (auto i = ++_e.begin(); i != _e.end(); ++i)
			_out << "," << *i;
	}
	_out << "]";
	return _out;
}
template <class T> inline std::ostream& operator<<(std::ostream& _out, std::list<T> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T, class U>
inline S& streamout(S& _out, std::pair<T, U> const& _e)
{
	_out << "(" << _e.first << "," << _e.second << ")";
	return _out;
}
template <class T, class U> inline std::ostream& operator<<(std::ostream& _out, std::pair<T, U> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T1, class T2, class T3>
inline S& streamout(S& _out, std::tuple<T1, T2, T3> const& _t)
{
	_out << "(" << std::get<0>(_t) << "," << std::get<1>(_t) << "," << std::get<2>(_t) << ")";
	return _out;
}
template <class T1, class T2, class T3> inline std::ostream& operator<<(std::ostream& _out, std::tuple<T1, T2, T3> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T, class U>
S& streamout(S& _out, std::map<T, U> const& _v)
{
	if (_v.empty())
		return _out << "{}";
	int i = 0;
	foreach (auto p, _v)
		_out << (!(i++) ? "{ " : "; ") << p.first << " => " << p.second;
	return _out << " }";
}
template <class T, class U> inline std::ostream& operator<<(std::ostream& _out, std::map<T, U> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T, class U>
S& streamout(S& _out, std::unordered_map<T, U> const& _v)
{
	if (_v.empty())
		return _out << "{}";
	int i = 0;
	foreach (auto p, _v)
		_out << (!(i++) ? "{ " : "; ") << p.first << " => " << p.second;
	return _out << " }";
}
template <class T, class U> inline std::ostream& operator<<(std::ostream& _out, std::unordered_map<T, U> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T>
S& streamout(S& _out, std::set<T> const& _v)
{
	if (_v.empty())
		return _out << "{}";
	int i = 0;
	foreach (auto p, _v)
		_out << (!(i++) ? "{ " : ", ") << p;
	return _out << " }";
}
template <class T> inline std::ostream& operator<<(std::ostream& _out, std::set<T> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T>
S& streamout(S& _out, std::multiset<T> const& _v)
{
	if (_v.empty())
		return _out << "{}";
	int i = 0;
	foreach (auto p, _v)
		_out << (!(i++) ? "{ " : ", ") << p;
	return _out << " }";
}
template <class T> inline std::ostream& operator<<(std::ostream& _out, std::multiset<T> const& _e) { streamout(_out, _e); return _out; }

template <class S, class T, class U>
S& streamout(S& _out, std::multimap<T, U> const& _v)
{
	if (_v.empty())
		return _out << "{}";
	T l;
	int i = 0;
	foreach (auto p, _v)
		if (!(i++))
			_out << "{ " << (l = p.first) << " => " << p.second;
		else if (l == p.first)
			_out << ", " << p.second;
		else
			_out << "; " << (l = p.first) << " => " << p.second;
	return _out << " }";
}
template <class T, class U> inline std::ostream& operator<<(std::ostream& _out, std::multimap<T, U> const& _e) { streamout(_out, _e); return _out; }

template <class _S, class _T> _S& operator<<(_S& _out, std::shared_ptr<_T> const& _p) { if (_p) _out << "@" << (*_p); else _out << "nullptr"; return _out; }

}
