#pragma once

namespace Lightbox
{

struct  Metrics
{
	Metrics();

	void incDraw()
	{
#if !LIGHTBOX_FINAL
		m_drawCount++;
#endif
	}

	void incUseProgram()
	{
#if !LIGHTBOX_FINAL
		m_useProgramCount++;
#endif
	}

	void reset();

#if !LIGHTBOX_FINAL
	unsigned m_useProgramCount;
	unsigned m_drawCount;
#endif
};

extern Metrics g_metrics;

}


