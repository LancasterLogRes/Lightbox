#include <Common/Global.h>
#include "GraphSpec.h"
#include "EventCompilerImpl.h"
using namespace std;
using namespace lb;

void EventCompilerImpl::doInit(Time _hop, Time _nyquist)
{
	m_graphsX.clear();
	m_hop = _hop;
	m_nyquist = _nyquist;
	m_t = 0;
	for (auto g: m_graphs)
		g.second->preinit();
	initPres();
	init();
	for (auto g: m_graphs)
		g.second->init();
}

void EventCompilerImpl::registerGraph(GraphSpec* _g)
{
	assert(!m_graphs.count(_g->name()));
	m_graphs.insert(std::make_pair(_g->name(), _g));
}

void EventCompilerImpl::addGraph(GenericCompute const& _c, GraphMetadata _g)
{
	ComputeRegistrar::get()->store(_c, false);
	_g.setOperationKey(_c.hash());
	if (_g.title().empty())
		_g.setTitle(_c.name());
	m_graphsX.push_back(_g);
}
