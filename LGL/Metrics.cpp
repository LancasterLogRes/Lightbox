#include <Common/Global.h>
#include "Global.h"
#include "Metrics.h"
using namespace std;
using namespace Lightbox;

Metrics Lightbox::g_metrics;

Metrics::Metrics():
	m_useProgramCount(0),
	m_drawCount(0)
{
}

void Metrics::reset()
{
	m_useProgramCount = 0;
	m_drawCount = 0;
}
