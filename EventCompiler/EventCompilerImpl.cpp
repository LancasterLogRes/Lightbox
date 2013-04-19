#include <Common/Global.h>
#include "EventCompilerImpl.h"
using namespace std;
using namespace Lightbox;

StreamEvents EventCompilerImpl::doInit(unsigned _bands, Time _hop, Time _nyquist)
{
	m_bands = _bands;
	m_hop = _hop;
	m_nyquist = _nyquist;
	m_t = 0;
	m_windowSeconds = toSeconds(windowSize());
	initPres();
	for (CompilerGraph* g: m_graphs)
		g->init();
	return init();
}

