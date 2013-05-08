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

namespace Lightbox
{

inline std::string id(float _y) { return toString(_y); }
inline std::string ms(float _x){ return toString(round(_x * 1000)) + (!_x ? "ms" : ""); }
inline std::string msL(float _x, float _y) { return toString(round(_x * 1000)) + "ms (" + toString(round(_y * 100)) + "%)"; }

#define LIGHTBOX_PREPROCESSORS(...) \
	virtual void initPres() { Initer(this) , __VA_ARGS__; } \
	virtual void executePres(Time _t, std::vector<Native> const& _mag, std::vector<Native> const& _phase, std::vector<Native> const& _wave) { Executor<Native>(this, _t, _mag, _phase, _wave) , __VA_ARGS__; } \
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
	Executor(EventCompilerImpl* _eci, Time _t, std::vector<_Native> const& _mag, std::vector<_Native> const& _phase, std::vector<_Native> const& _wave): m_eci(_eci), m_t(_t), m_mag(_mag), m_phase(_phase), m_wave(_wave)
	{
	}
	template <class _T> Executor& operator,(_T& _t) { _t.execute(m_eci, m_t, m_mag, m_phase, m_wave); return *this; }
	EventCompilerImpl* m_eci;
	Time m_t;
	std::vector<_Native> const& m_mag;
	std::vector<_Native> const& m_phase;
	std::vector<_Native> const& m_wave;
};

class CompilerGraph;

class EventCompilerImpl
{
	friend class EventCompiler;
	template <class _Native> friend class NullEventCompiler;

public:
	typedef EventCompilerImpl LIGHTBOX_STATE_BaseClass;		// Used by the LIGHTBOX_STATE State collector.
	typedef EventCompilerImpl LIGHTBOX_PROPERTIES_BaseClass;	// Used for the LIGHTBOX_PROPERTIES Members collector.

	EventCompilerImpl() {}
	virtual ~EventCompilerImpl() {}

	inline unsigned bands() const { return m_bands; }
	inline Time windowSize() const { return toBase((m_bands - 1) * 2, rate()); }
	inline Time hop() const { return m_hop; }
	inline Time nyquist() const { return m_nyquist; }
	inline unsigned rate() const { return s_baseRate / m_nyquist * 2; }
	inline float band(float _frequency) const { return clamp<float>(_frequency * m_windowSeconds, 0.f, m_bands - 1.f); }

	StreamEvents doInit(unsigned _bands, Time _hop, Time _nyquist);

	virtual StreamEvents init() { return StreamEvents(); }
	virtual MemberMap propertyMap() const { return NullMemberMap; }
	virtual MemberMap stateMap() const { return NullMemberMap; }

	virtual StreamEvents doCompile(std::vector<float> const& _mag, std::vector<float> const& _phase, std::vector<float> const& _wave) = 0;
	virtual StreamEvents doCompile(std::vector<Fixed16> const& _mag, std::vector<Fixed16> const& _phase, std::vector<Fixed16> const& _wave) = 0;
	virtual StreamEvents doCompile(std::vector<Fixed<11, 21>> const& _mag, std::vector<Fixed<11, 21>> const& _phase, std::vector<Fixed<11, 21>> const& _wave) = 0;

	template <class _Out, class _This, class _In> static StreamEvents crossCompile(_This* _this, std::vector<_In> const& _mag, std::vector<_In> const& _phase, std::vector<_In> const& _wave, typename std::enable_if<!std::is_same<_Out, _In>::value>::type* = 0)
	{
		std::vector<_Out> mag(_mag.size());
		std::vector<_Out> phase(_phase.size());
		std::vector<_Out> wave(_wave.size());
		for (unsigned i = 0; i < _mag.size(); ++i)
			mag[i] = (_Out)_mag[i];
		for (unsigned i = 0; i < _phase.size(); ++i)
			phase[i] = (_Out)_phase[i];
		for (unsigned i = 0; i < _wave.size(); ++i)
			wave[i] = (_Out)_wave[i];
		return crossCompile<_Out>(_this, mag, phase, wave);
	}

	template <class _InOut, class _This> static StreamEvents crossCompile(_This* _this, std::vector<_InOut> const& _mag, std::vector<_InOut> const& _phase, std::vector<_InOut> const& _wave)
	{
		EventCompilerImpl* thisBase = _this;
		_this->executePres(thisBase->m_t, _mag, _phase, _wave);
		StreamEvents ret = _this->compile(thisBase->m_t, _mag, _phase, _wave);
		thisBase->m_t += thisBase->m_hop;
		return ret;
	}

	inline Time time() const { return m_t; }
	inline unsigned index() const { return m_t / m_hop; }
	void registerGraph(CompilerGraph* _g) { m_graphs.push_back(_g); }
	std::vector<CompilerGraph*> const& graphs() const { return m_graphs; }

protected:
	virtual void initPres() {}

private:
	Time m_hop;
	Time m_nyquist;
	unsigned m_bands;
	Time m_t;

	// Cached...
	float m_windowSeconds;

	std::vector<CompilerGraph*> m_graphs;
};

class CompilerGraph
{
public:
	CompilerGraph(EventCompilerImpl* _ec, std::string const& _name): m_ec(_ec), m_name(_name)
	{
		m_ec->registerGraph(this);
	}
	virtual ~CompilerGraph() {}

	EventCompilerImpl* ec() const { return m_ec; }
	std::string const& name() const { return m_name; }

	virtual void init() {}

protected:
	EventCompilerImpl* m_ec;
	std::string m_name;
};

template <class _N>
class XO
{
public:
	XO(_N _scale = 1, _N _offset = 0): m_scale(_scale), m_offset(_offset) {}
	static XO toUnity(Range _r) { return XO(1 / (_r.second - _r.first), _r.first / (_r.second - _r.first)); }

	_N apply(_N _v) const { return _v * m_scale + m_offset; }
	Range apply(Range _v) const { return Range(apply(_v.first), apply(_v.second)); }

	_N scale() const { return m_scale; }
	_N offset() const { return m_offset; }

private:
	_N m_scale;
	_N m_offset;
};

typedef XO<float> XOf;

class GraphChart: public CompilerGraph
{
public:
	GraphChart(EventCompilerImpl* _ec, std::string const& _name): CompilerGraph(_ec, _name) {}
	template <class ... _P> GraphChart(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): GraphChart(_ec, _name) { setup(_p ...); }

	void setup(std::string const& _ylabel = "", XOf _ytx = XOf(), Range _yrangeHint = AutoRange)
	{
		m_ylabel = _ylabel;
		m_ytx = _ytx;
		m_yrangeHint = _yrangeHint;
	}

	std::vector<float> const& data() const { return m_data; }

	std::string ylabel() const { return m_ylabel; }

	Range yrangeReal() const { return m_yrangeReal; }
	Range yrangeHint() const { return m_yrangeHint; }

	virtual void init()
	{
		m_data.clear();
		m_yrangeReal = AutoRange;
	}

	template <class _T> void operator<<(_T const& _a) { shift(_a); }
	template <class _T> void shift(_T const& _a)
	{
		unsigned i = ec()->index();
		float l = m_data.size() ? m_data.back() : _a;
		widenToFit(m_yrangeReal, _a);
		m_data.reserve(i + 1);
		while (m_data.size() < i)
			m_data.push_back(l);
		m_data.push_back((float)_a);
	}

private:
	std::string m_ylabel;
	XOf m_ytx;
	Range m_yrangeHint;

	Range m_yrangeReal;
	std::vector<float> m_data;
};

class GraphSpectrum: public CompilerGraph
{
public:
	GraphSpectrum(EventCompilerImpl* _ec, std::string const& _name): CompilerGraph(_ec, _name) {}
	template <class ... _P> GraphSpectrum(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): CompilerGraph(_ec, _name) { setup(_p ...); }

	void setup(unsigned _bandCount, std::string _xlabel = "", std::string _ylabel = "", XOf _xtx = XOf(), XOf _ytx = XOf(), Range _yrangeHint = AutoRange)
	{
		m_bandCount = _bandCount;
		m_xlabel = _xlabel;
		m_ylabel = _ylabel;
		m_xtx = _xtx;
		m_ytx = _ytx;
		m_yrangeHint = _yrangeHint;
	}

	std::map<Time, std::vector<float> > const& data() const { return m_data; }
	unsigned bandCount() const { return m_bandCount; }

	XOf xtx() const { return m_xtx; }

	std::string xlabel() const { return m_xlabel; }
	std::string ylabel() const { return m_ylabel; }

	Range xrangeReal() const { return m_xrangeReal; }
	Range yrangeReal() const { return m_yrangeReal; }
	Range yrangeHint() const { return m_yrangeHint; }

	virtual void init()
	{
		m_data.clear();
		m_xrangeReal = m_xtx.apply(std::make_pair(0, m_bandCount - 1));
		m_yrangeReal = AutoRange;
	}

	template <class _T> void operator<<(_T const& _a) { shift(_a); }
	template <class _T> void shift(_T const& _a)
	{
		assert(_a.size() == m_bandCount);
		m_data[m_ec->time()].resize(m_bandCount);
		float* f = m_data[m_ec->time()].data();
		for (auto t: _a)
		{
			float v = m_ytx.apply((float)t);
			if (isFinite(m_yrangeReal.first))
				widenToFit(m_yrangeReal, v);
			else
				m_yrangeReal = std::make_pair(v, v);
			*f++ = v;
		}
	}

private:
	unsigned m_bandCount;
	std::string m_xlabel;
	std::string m_ylabel;
	XOf m_xtx;
	XOf m_ytx;
	Range m_yrangeHint;

	Range m_xrangeReal;
	Range m_yrangeReal;
	std::map<Time, std::vector<float>> m_data;
};

template <class _Native>
class NullEventCompiler: public EventCompilerImpl
{
	friend class EventCompilerImpl;

public:
	typedef _Native Native;

	Time time() const { return m_t; }

protected:
	virtual StreamEvents doCompile(std::vector<float> const&, std::vector<float> const&, std::vector<float> const&) { m_t += m_hop; return StreamEvents(); }
	virtual StreamEvents doCompile(std::vector<Fixed16> const&, std::vector<Fixed16> const&, std::vector<Fixed16> const&) { m_t += m_hop; return StreamEvents(); }
	virtual StreamEvents doCompile(std::vector<Fixed<11, 21>> const&, std::vector<Fixed<11, 21>> const&, std::vector<Fixed<11, 21>> const&) { m_t += m_hop; return StreamEvents(); }
};

template <class _Native>
class EventCompilerNativeImpl: public EventCompilerImpl
{
	friend class EventCompilerImpl;

public:
	typedef _Native Native;

protected:
	virtual StreamEvents compile(Time _t, std::vector<Native> const& _mag, std::vector<Native> const& _phase, std::vector<Native> const& _wave) { (void)_t; (void)_mag; (void)_phase; (void)_wave; return StreamEvents(); }
	virtual void executePres(Time _t, std::vector<Native> const& _mag, std::vector<Native> const& _phase, std::vector<Native> const& _wave) { (void)_t; (void)_mag; (void)_phase; (void)_wave; }

private:
	virtual StreamEvents doCompile(std::vector<float> const& _mag, std::vector<float> const& _phase, std::vector<float> const& _wave) { return crossCompile<Native>(this, _mag, _phase, _wave); }
	virtual StreamEvents doCompile(std::vector<Fixed16> const& _mag, std::vector<Fixed16> const& _phase, std::vector<Fixed16> const& _wave) { return crossCompile<Native>(this, _mag, _phase, _wave); }
	virtual StreamEvents doCompile(std::vector<Fixed<11, 21>> const& _mag, std::vector<Fixed<11, 21>> const& _phase, std::vector<Fixed<11, 21>> const& _wave) { return crossCompile<Native>(this, _mag, _phase, _wave); }
};

}
