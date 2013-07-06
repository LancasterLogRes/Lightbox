#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "PeakTrack.h"
using namespace std;
using namespace lb;

void PeakTrackImpl::init()
{
	m_peaks.resize(count);
	for (auto& p: m_peaks)
		p.band = rand() * input.info().bands / RAND_MAX;
	sort(m_peaks.begin(), m_peaks.end());
}

void PeakTrackImpl::compute(std::vector<FreqPeak>& _v)
{
	_v.resize(count);

	foreign_vector<Peak<>> peaks = input.get();

	for (int pi = peaks.size() - 1; pi >= 0; --pi)
	{
		int bestSi = -1;
		PeakState bestS;
		float best;

		Peak<> p = peaks[pi];
		for (unsigned si = 0; si < m_peaks.size(); ++si)
		{
			PeakState s = m_peaks[si];
			// Attract s to peaks[pi]
			s.attract(p, gravity);
			float f = s.fit(p);
			if (bestSi == -1 || f < best)
			{
				bestSi = si;
				bestS = s;
				best = f;
			}
		}
		m_peaks[bestSi] = bestS;
	}

	for (auto it = m_peaks.begin(); it != m_peaks.end(); it++)
	{
		/*if (it == m_peaks.begin())
		{
			auto np = *next(it);
			it->move(llerp(lext(it->mag, np.mag), 0.f, np.band), gravity);
		}
		else if (it == prev(m_peaks.end()))
		{
			auto pp = *prev(it);
			it->move(llerp(lext(pp.mag, it->mag), pp.band, (float)m.size()), gravity);
		}
		else
		{
			auto pp = *prev(it);
			auto np = *next(it);
			it->move(llerp(1.f - lext(pp.mag, np.mag), pp.band, np.band), gravity);
		}*/

		it->phase += 0.1;
		it->mag *= 0.95;
	}
	valcpy(_v.data(), (FreqPeak*)m_peaks.data(), count);
}
