#include <Common/Global.h>
#include "Global.h"
#include "Metrics.h"
using namespace std;
using namespace Lightbox;

Metrics Lightbox::g_metrics;

Metrics::Metrics()
{
	reset();
}

void Metrics::reset()
{
#if !LIGHTBOX_FINAL
	m_useProgramCount = 0;
	m_drawCount = 0;
#endif
}
