#pragma once

namespace Lightbox
{

struct  Metrics
{
	Metrics();

	void reset();

	unsigned m_useProgramCount;
	unsigned m_drawCount;
};

extern Metrics g_metrics;

}


