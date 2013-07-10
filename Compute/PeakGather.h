#pragma once

#include <vector>
#include <Common/Peaks.h>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class PeakGatherImpl: public ComputeImplBase<Peak<>, SpectrumInfo>
{
public:
	PeakGatherImpl(Compute<float, SpectrumInfo> const& _input, unsigned _count): ComputeImplBase(input, count), input(_input), count(_count) {}
	virtual ~PeakGatherImpl() {}

	virtual char const* name() const { return "PeakGather"; }
	virtual SpectrumInfo info() { return input.info(); }
	virtual void init();
	virtual void compute(std::vector<Peak<>>& _v);

	Compute<float, SpectrumInfo> input;
	unsigned count;
};

using PeakGather = ComputeBase<PeakGatherImpl>;

}


