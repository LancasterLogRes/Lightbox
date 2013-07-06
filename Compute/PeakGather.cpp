#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "PeakGather.h"
using namespace std;
using namespace lb;

void PeakGatherImpl::init()
{
}

void PeakGatherImpl::compute(std::vector<Peak<>>& _v)
{
	_v.resize(count);
	auto m = input.get();
	unsigned peakCount = topPeaks(m, foreign_vector<Peak<>>(&_v));
	_v.resize(peakCount);
}


