#pragma once

#include "Common.h"
#include "Compute.h"
#include "Accumulate.h"
#include "Bark.h"
#include "CycleDelta.h"
#include "ExtractMagnitude.h"
#include "ExtractPhase.h"
#include "PeakGather.h"
#include "MeanRecord.h"
#include "PeakTrack.h"
#include "SumRecord.h"
#include "WindowedFourier.h"
#include "ZeroCrossings.h"


// Compositions

namespace lb
{

class LoudnessImpl: public MeanRecordImpl
{
public:
	LoudnessImpl(Compute<float, PCMInfo> const& _input, unsigned _factor = 8, WindowFunction _function = HannWindow, float _functionParam = 0, bool _zeroPhase = true):
		MeanRecordImpl(Compute<float, VoidInfo>(BarkPhon(ExtractMagnitude(WindowedFourier(AccumulateWave(_input, _factor), _function, _functionParam, _zeroPhase))))) {}
};
using Loudness = ComputeBase<LoudnessImpl>;

}
