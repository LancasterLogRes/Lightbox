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

#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>
#include <boost/variant.hpp>
#include <Common/Global.h>
#include <Common/MemberCollection.h>
#include <Common/Time.h>
#include <Common/StreamIO.h>
#include "StreamEvent.h"

namespace lb
{

inline std::string id(float _y) { return toString(_y); }
inline std::string idL(float _x, float _y) { return toString(_x) + " (" + toString(round(_y * 100)) + "%)"; }
inline std::string ms(float _x){ return toString(round(_x * 1000)) + (!_x ? "ms" : ""); }
inline std::string msL(float _x, float _y) { return toString(round(_x * 1000)) + "ms (" + toString(round(_y * 100)) + "%)"; }

#define LIGHTBOX_PREPROCESSORS(...) \
	virtual void initPres() { Initer(this) , __VA_ARGS__; } \
	virtual void executePres(std::vector<Native> const& _wave) { Executor<Native>(this, _wave) , __VA_ARGS__; } \
	class Lightbox_Preprocessors_Macro_End {}

class EventCompiler;
class EventCompilerImpl;

struct Initer
{
	Initer(EventCompilerImpl* _eci): m_eci(_eci) {}
	template <class _T> Initer& operator,(_T& _t) { _t.init(m_eci); return *this; }
	EventCompilerImpl* m_eci;
};

template <class _Native>
struct Executor
{
	Executor(EventCompilerImpl* _eci, std::vector<_Native> const& _wave): m_eci(_eci), m_wave(_wave)
	{
	}
	template <class _T> Executor& operator,(_T& _t) { _t.execute(m_eci, m_wave); return *this; }
	EventCompilerImpl* m_eci;
	std::vector<_Native> const& m_wave;
};

class GraphSpec;

class EventCompilerImpl
{
	friend class EventCompiler;
	template <class _Native> friend class NullEventCompiler;

public:
	typedef EventCompilerImpl LIGHTBOX_STATE_BaseClass;		// Used by the LIGHTBOX_STATE State collector.
	typedef EventCompilerImpl LIGHTBOX_PROPERTIES_BaseClass;	// Used for the LIGHTBOX_PROPERTIES Members collector.

	EventCompilerImpl() {}
	virtual ~EventCompilerImpl() {}

	inline Time hop() const { return m_hop; }
	inline Time nyquist() const { return m_nyquist; }
	inline unsigned rate() const { return s_baseRate * 2 / m_nyquist; }

	void doInit(Time _hop, Time _nyquist);

	virtual void init() {}
	virtual MemberMap propertyMap() const { return NullMemberMap; }
	virtual MemberMap stateMap() const { return NullMemberMap; }

	virtual StreamEvents doCompile(std::vector<float> const& _wave) = 0;
	virtual StreamEvents doCompile(std::vector<Fixed16> const& _wave) = 0;
	virtual StreamEvents doCompile(std::vector<Fixed<11, 21>> const& _wave) = 0;

	template <class _Out, class _This, class _In> static StreamEvents crossCompile(_This* _this, std::vector<_In> const& _wave, typename std::enable_if<!std::is_same<_Out, _In>::value>::type* = 0)
	{
		std::vector<_Out> wave(_wave.size());
		for (unsigned i = 0; i < _wave.size(); ++i)
			wave[i] = (_Out)_wave[i];
		return crossCompile<_Out>(_this, wave);
	}

	template <class _InOut, class _This> static StreamEvents crossCompile(_This* _this, std::vector<_InOut> const& _wave)
	{
		EventCompilerImpl* thisBase = _this;
		_this->executePres(_wave);
		StreamEvents ret = _this->compile(thisBase->m_t, _wave);
		thisBase->m_t += thisBase->m_hop;
		return ret;
	}

	inline Time time() const { return m_t; }
	inline unsigned index() const { return m_t / m_hop; }
	void registerGraph(GraphSpec* _g);
	std::vector<GraphSpec*> graphs() const { return values(m_graphs); }
	std::map<std::string, GraphSpec*> const& graphMap() const { return m_graphs; }
	GraphSpec* graph(std::string _name) const { if (m_graphs.count(_name)) return m_graphs.at(_name); return nullptr; }

protected:
	virtual void initPres() {}

private:
	Time m_hop;
	Time m_nyquist;
	Time m_t;

	std::map<std::string, GraphSpec*> m_graphs;
};

template <class _Native>
class NullEventCompiler: public EventCompilerImpl
{
	friend class EventCompilerImpl;

public:
	typedef _Native Native;

	Time time() const { return m_t; }

protected:
	virtual StreamEvents doCompile(std::vector<float> const&) { m_t += m_hop; return StreamEvents(); }
	virtual StreamEvents doCompile(std::vector<Fixed16> const&) { m_t += m_hop; return StreamEvents(); }
	virtual StreamEvents doCompile(std::vector<Fixed<11, 21>> const&) { m_t += m_hop; return StreamEvents(); }
};

template <class _Native>
class EventCompilerNativeImpl: public EventCompilerImpl
{
	friend class EventCompilerImpl;

public:
	typedef _Native Native;

protected:
	virtual StreamEvents compile(Time _t, std::vector<Native> const& _wave) { (void)_t; (void)_wave; return StreamEvents(); }
	virtual void executePres(std::vector<Native> const& _wave) { (void)_wave; }

private:
	virtual StreamEvents doCompile(std::vector<float> const& _wave) { return crossCompile<Native>(this, _wave); }
	virtual StreamEvents doCompile(std::vector<Fixed16> const& _wave) { return crossCompile<Native>(this, _wave); }
	virtual StreamEvents doCompile(std::vector<Fixed<11, 21>> const& _wave) { return crossCompile<Native>(this, _wave); }
};

}
