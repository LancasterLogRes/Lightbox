#pragma once

#include <string>
#include <set>
#include <vector>

#include <Common/Trivial.h>

#include "EventCompilerImpl.h"

namespace lb
{

// Something that stores some timeline data.
// Will be implemented with DataSet, but this is virtualised so it doesn't have to be in EventCompiler.
class DataStore
{
public:
	virtual ~DataStore() {}

	// Variable record length if 0. _dense if all hops are stored, otherwise will store sparsely.
	virtual void init(unsigned _recordLength, bool _dense) { (void)_recordLength; (void)_dense; }
	virtual void shiftBuffer(unsigned _index, foreign_vector<float> const& _record) { (void)_index; (void)_record; }
};

class GraphSpec
{
public:
	GraphSpec(GraphSpec& _parent, std::string const& _name): m_ec(_parent.ec()), m_parent(&_parent), m_name(_name)
	{
		if (m_ec)
			m_ec->registerGraph(this);
		m_parent->m_children.insert(this);
	}
	GraphSpec(EventCompilerImpl* _ec, std::string const& _name): m_ec(_ec), m_parent(nullptr), m_name(_name)
	{
		if (m_ec)
			m_ec->registerGraph(this);
	}
	virtual ~GraphSpec() {}

	GraphSpec* parent() const { return m_parent; }
	std::set<GraphSpec*> const& children() const { return m_children; }
	EventCompilerImpl* ec() const { return m_ec; }
	std::string const& name() const { return m_name; }
	void setStore(DataStore* _s) { m_store = _s; }

	virtual void preinit()
	{
		m_doneSetupThisTime = false;
	}

	virtual void init()
	{
		if (m_parent && (!m_doneSetup || (!m_doneSetupThisTime && m_parent->m_doneSetupThisTime)))
		{
			setupFromParent();
			m_doneSetup = m_doneSetupThisTime = true;
		}
		if (!m_doneSetup)
		{
			cwarn << "GraphSpec" << m_name << "not set up!";
		}
	}

	void setup()
	{
		m_doneSetup = m_doneSetupThisTime = true;
	}

	virtual void setupFromParent() {}

protected:
	EventCompilerImpl* m_ec;
	GraphSpec* m_parent;
	std::set<GraphSpec*> m_children;

	std::string m_name;

	bool m_doneSetup = false;
	bool m_doneSetupThisTime = false;

	DataStore* m_store = nullptr;
};

class GraphChart: public GraphSpec
{
public:
	GraphChart(GraphChart& _p, std::string const& _name): GraphSpec(_p, _name) {}
	GraphChart(EventCompilerImpl* _ec, std::string const& _name): GraphSpec(_ec, _name) {}
	template <class ... _P> GraphChart(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): GraphChart(_ec, _name) { setup(_p ...); }

	void setup(std::string const& _ylabel = "", XOf _ytx = XOf(), Range _yrangeHint = AutoRange)
	{
		GraphSpec::setup();
		m_ylabel = _ylabel;
		m_ytx = _ytx;
		m_yrangeHint = _yrangeHint;
	}
	virtual void setupFromParent() { GraphSpec::setupFromParent(); if (auto p = dynamic_cast<GraphChart*>(parent())) { m_ylabel = p->m_ylabel, m_ytx = p->m_ytx, m_yrangeHint = p->m_yrangeHint; } }

	std::vector<float> const& data() const { return m_data; }

	std::string ylabel() const { return m_ylabel; }
	Range yrangeReal() const { return m_yrangeReal; }
	Range yrangeHint() const { return m_yrangeHint; }

	virtual void init()
	{
		GraphSpec::init();
		m_data.clear();
		m_yrangeReal = AutoRange;
		if (m_store)
			m_store->init(1, true);
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

		if (m_store)
		{
			float d = (float)_a;
			m_store->shiftBuffer(ec()->index(), lb::foreign_vector<float>(d, lb::ByValue));
		}
	}

private:
	std::string m_ylabel;
	XOf m_ytx;
	Range m_yrangeHint;

	Range m_yrangeReal;
	std::vector<float> m_data;
};

class GraphSparseDense: public GraphSpec
{
public:
	GraphSparseDense(GraphSparseDense& _ec, std::string const& _name): GraphSpec(_ec, _name) {}
	GraphSparseDense(EventCompilerImpl* _ec, std::string const& _name): GraphSpec(_ec, _name) {}

	void setup(std::string _xlabel = "", std::string _ylabel = "", XOf _xtx = XOf(), XOf _ytx = XOf(), Range _yrangeHint = AutoRange)
	{
		GraphSpec::setup();
		m_xlabel = _xlabel;
		m_ylabel = _ylabel;
		m_xtx = _xtx;
		m_ytx = _ytx;
		m_yrangeHint = _yrangeHint;
	}
	virtual void setupFromParent() { GraphSpec::setupFromParent(); if (auto p = dynamic_cast<GraphSparseDense*>(parent())) { m_xlabel = p->m_xlabel; m_ylabel = p->m_ylabel, m_xtx = p->m_xtx, m_ytx = p->m_ytx, m_yrangeHint = p->m_yrangeHint; } }

	std::vector<float> const& dataPoint(int _index) const { auto it = m_data.upper_bound(_index); if (it == m_data.begin()) return NullVectorFloat; else return (--it)->second; }
	std::map<int, std::vector<float> > const& data() const { return m_data; }

	XOf xtx() const { return m_xtx; }

	std::string xlabel() const { return m_xlabel; }
	std::string ylabel() const { return m_ylabel; }

	virtual Range xrangeReal() const = 0;
	virtual Range xrangeHint() const { return AutoRange; }
	Range yrangeReal() const { return m_yrangeReal; }
	Range yrangeHint() const { return m_yrangeHint; }

	virtual void init()
	{
		GraphSpec::init();
		m_data.clear();
		m_yrangeReal = AutoRange;
	}

	template <class _T> void shift(_T const& _a, int _offset = 0)
	{
		m_data[m_ec->index()].resize(_a.size());

		float* f = m_data[m_ec->index()].data();
		unsigned s = _a.size();
		unsigned i = (s - _offset) % s;
		for (auto t: _a)
		{
			f[i] = m_ytx.apply((float)t);
			i++;
			if (i == s)
				i = 0;
		}

		if (m_store)
		{
			float f[_a.size()];

			unsigned i = (s - _offset) % s;
			for (auto t: _a)
			{
				f[i] = m_ytx.apply((float)t);
				if (isFinite(m_yrangeReal.first))
					widenToFit(m_yrangeReal, f[i]);
				else
					m_yrangeReal = std::make_pair(f[i], f[i]);
				i++;
				if (i == s)
					i = 0;
			}

			m_store->shiftBuffer(ec()->index(), foreign_vector<float>(&*f, _a.size()));
		}
	}

private:
	std::string m_xlabel;
	std::string m_ylabel;
	XOf m_xtx;
	XOf m_ytx;
	Range m_yrangeHint;

	Range m_yrangeReal;
	std::map<int, std::vector<float>> m_data;
};

class GraphDenseDenseFixed: public GraphSpec
{
public:
	GraphDenseDenseFixed(GraphDenseDenseFixed& _ec, std::string const& _name): GraphSpec(_ec, _name) {}
	GraphDenseDenseFixed(EventCompilerImpl* _ec, std::string const& _name): GraphSpec(_ec, _name) {}
	template <class ... _P> GraphDenseDenseFixed(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): GraphSpec(_ec, _name) { setup(_p ...); }

	void setup(unsigned _graphSize, std::string _xlabel = "", std::string _ylabel = "", XOf _xtx = XOf(), XOf _ytx = XOf(), Range _yrangeHint = AutoRange)
	{
		GraphSpec::setup();
		m_graphSize = _graphSize;
		m_xlabel = _xlabel;
		m_ylabel = _ylabel;
		m_xtx = _xtx;
		m_ytx = _ytx;
		m_yrangeHint = _yrangeHint;
	}
	virtual void setupFromParent() { GraphSpec::setupFromParent(); if (auto p = dynamic_cast<GraphDenseDenseFixed*>(parent())) { m_xlabel = p->m_xlabel; m_ylabel = p->m_ylabel, m_xtx = p->m_xtx, m_ytx = p->m_ytx, m_yrangeHint = p->m_yrangeHint; m_graphSize = p->m_graphSize; } }

	XOf xtx() const { return m_xtx; }

	std::string xlabel() const { return m_xlabel; }
	std::string ylabel() const { return m_ylabel; }

	unsigned graphSize() const { return m_graphSize; }

	virtual Range xrangeReal() const { return xtx().apply(std::make_pair(0, m_graphSize - 1)); }
	virtual Range xrangeHint() const { return AutoRange; }
	Range yrangeReal() const { return m_yrangeReal; }
	Range yrangeHint() const { return m_yrangeHint; }

	virtual void init()
	{
		GraphSpec::init();
		m_yrangeReal = AutoRange;
		if (m_store)
			m_store->init(m_graphSize, true);
	}

	template <class _T> void shift(_T const& _a, int _offset = 0)
	{
		assert(_a.size() == m_graphSize);

		if (m_store)
		{
			float f[_a.size()];

			unsigned i = (m_graphSize - _offset) % m_graphSize;
			for (auto t: _a)
			{
				f[i] = m_ytx.apply((float)t);
				if (isFinite(m_yrangeReal.first))
					widenToFit(m_yrangeReal, f[i]);
				else
					m_yrangeReal = std::make_pair(f[i], f[i]);
				i++;
				if (i == m_graphSize)
					i = 0;
			}

			m_store->shiftBuffer(ec()->index(), foreign_vector<float>(&*f, _a.size()));
		}
	}

private:
	std::string m_xlabel;
	std::string m_ylabel;
	XOf m_xtx;
	XOf m_ytx;
	Range m_yrangeHint;

	Range m_yrangeReal;
	unsigned m_graphSize = 0;
};

class GraphHistogram: public GraphSparseDense
{
public:
	GraphHistogram(EventCompilerImpl* _ec, std::string const& _name): GraphSparseDense(_ec, _name) {}
	GraphHistogram(GraphHistogram& _ec, std::string const& _name): GraphSparseDense(_ec, _name) {}
	template <class ... _P> GraphHistogram(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): GraphSparseDense(_ec, _name) { setup(_p ...); }

	virtual void init()
	{
		GraphSparseDense::init();
		m_maxGraphSize = 0;
		if (m_store)
			m_store->init(0, false);
	}

	virtual Range xrangeReal() const { return xtx().apply(std::make_pair(0, m_maxGraphSize - 1)); }

	template <class _T> void operator<<(_T const& _a) { shift(_a); }
	template <class _T> void shift(_T const& _a, unsigned _offset = 0)
	{
		GraphSparseDense::shift(_a, _offset);
		m_maxGraphSize = std::max<unsigned>(m_maxGraphSize, _a.size());
	}

private:
	unsigned m_maxGraphSize;
};

class GraphSparseDenseConstSize: public GraphSparseDense
{
public:
	GraphSparseDenseConstSize(GraphSparseDenseConstSize& _ec, std::string const& _name): GraphSparseDense(_ec, _name) {}
	GraphSparseDenseConstSize(EventCompilerImpl* _ec, std::string const& _name): GraphSparseDense(_ec, _name) {}

	template <class ... _P>
	void setup(unsigned _graphSize, _P ... _p)
	{
		m_graphSize = _graphSize;
		GraphSparseDense::setup(_p ...);
	}
	virtual void setupFromParent() { GraphSparseDense::setupFromParent(); if (auto p = dynamic_cast<GraphSparseDenseConstSize*>(parent())) { m_graphSize = p->m_graphSize; } }

	virtual void init()
	{
		GraphSparseDense::init();
		if (m_store)
			m_store->init(m_graphSize, false);
	}

	unsigned graphSize() const { return m_graphSize; }
	virtual Range xrangeReal() const { return xtx().apply(std::make_pair(0, m_graphSize - 1)); }

	template <class _T> void shift(_T const& _a, unsigned _offset = 0)
	{
		assert(_a.size() == m_graphSize);
		GraphSparseDense::shift(_a, _offset);
	}

private:
	unsigned m_graphSize = 0;
};

class GraphHistory: public GraphSparseDenseConstSize
{
public:
	GraphHistory(GraphHistory& _ec, std::string const& _name): GraphSparseDenseConstSize(_ec, _name) {}
	GraphHistory(EventCompilerImpl* _ec, std::string const& _name): GraphSparseDenseConstSize(_ec, _name) {}
	template <class ... _P> GraphHistory(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): GraphSparseDenseConstSize(_ec, _name) { setup(_p ...); }

	unsigned historySize() const { return graphSize(); }

	template <class _T> void operator<<(_T const& _a) { shift(_a); }
};

class GraphSpectrum: public GraphSparseDenseConstSize
{
public:
	GraphSpectrum(GraphSpectrum& _ec, std::string const& _name): GraphSparseDenseConstSize(_ec, _name) {}
	GraphSpectrum(EventCompilerImpl* _ec, std::string const& _name): GraphSparseDenseConstSize(_ec, _name) {}
	template <class ... _P> GraphSpectrum(EventCompilerImpl* _ec, std::string const& _name, _P ... _p): GraphSparseDenseConstSize(_ec, _name) { setup(_p ...); }

	unsigned bandCount() const { return graphSize(); }

	template <class _T> void operator<<(_T const& _a) { shift(_a); }
};

}


