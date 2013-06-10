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

#include "Common.h"
#include "Analysis.h"
#include "Arithmetical.h"
#include "Bark.h"
#include "CrossComparison.h"
#include "Downsampled.h"
#include "Flow.h"
#include "Historical.h"
#include "Normalization.h"
#include "Peaks.h"
#include "Spectral.h"
#include "Surface.h"
#include "Tonal.h"

namespace lb
{

template <class _PP = MagnitudeComponent<>, unsigned _FromHz = 0, unsigned _ToHz = 96000>
class DeltaPower: public Deltad<Power<_PP, _FromHz, _ToHz>> {};

}

/*
// Chord
{
	array<float, 12> chord;
	chord.fill(0);
	float tmaga = 0;
	float tmagb = 0;
	float tsim = 0;
	for (unsigned t = 0; t < 12; ++t)
	{
		// calculate mt, the maximum i that is in range [_f, _t) for the tone t.
		for (unsigned o = 0; o < 8; ++o)
		{
			unsigned fi = max<int>(_f, (int)round(band(toneFrequency(t - .5f, (float)o))));
			unsigned ti = min<int>(_t, (int)round(band(toneFrequency(t + .5f, (float)o))));
			if (fi < _t || ti >= _f)
				for (unsigned i = fi; i < ti; ++i)
					chord[o] += sqr((1.f - fabs(mod1(_phase[i] - m_phase[i] + 1) - mod1(m_phase[i] - m_lphase[i] + 1))) * _mag[i]);
		}

		tsim += m_lastChord[_b][t] * chord[t];
		tmaga += sqr(m_lastChord[_b][t]);
		tmagb += sqr(chord[t]);
	}
	float tot = 0;
	for (unsigned t = 0; t < 12; ++t)
	{
		m_lastChord[_b][t] = max(0.00000001f, m_lastChord[_b][t] * chord[t]);
		tot += m_lastChord[_b][t];
	}
	float scale = 1.f / tot;
	for (unsigned t = 0; t < 12; ++t)
		m_lastChord[_b][t] *= scale;

	return tsim / (sqrt(tmaga) * sqrt(tmagb));
}
*/

/*

	vector<float> lastPhase;
	vector<float> deltaPhase;

: phaseEntropyAvg(12),

deltaPhase.resize(bands(), -1);
lastPhase.resize(bands(), -1);
phaseEntropyAvg.init();
phaseEntropyC.setHistory(c_historySize, downsampleST);
*/
/*
		float phaseEntropy = 0.f;
		if (lastPhase[0] == -1)
			lastPhase = _phase;
		else if (deltaPhase[0] == -1)
			for (unsigned i = 0; i < bands(); ++i)
				deltaPhase[i] = withReflection(abs(_phase[i] - lastPhase[i]));
		else
		{
			for (unsigned i = 0; i < bands(); ++i)
			{
				float dp = withReflection(abs(_phase[i] - lastPhase[i]));
				lastPhase[i] = _phase[i];
				phaseEntropy += max<float>(0, withReflection(abs(dp - deltaPhase[i])) - halfPi<float>());
				deltaPhase[i] = dp;
			}
			ret.push_back(StreamEvent(Graph, phaseEntropy, 0.08f));
		}
		phaseEntropyAvg.push(phaseEntropy);
*/

