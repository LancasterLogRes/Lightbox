#include <Common/Global.h>
#include "GraphSpec.h"
#include "EventCompilerImpl.h"
using namespace std;
using namespace lb;

void EventCompilerImpl::doInit(Time _hop, Time _nyquist)
{
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
