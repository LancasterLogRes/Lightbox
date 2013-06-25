#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class ExtractPhaseImpl: public ComputeImplBase<SpectrumInfo, float>
{
public:
	ExtractPhaseImpl(Compute<ComplexSpectrumInfo, float> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~ExtractPhaseImpl() {}
	
	virtual char const* name() const { return "ExtractPhase"; }
	virtual SpectrumInfo info() { return input.info(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<ComplexSpectrumInfo, float> input;
};

using ExtractPhase = ComputeBase<ExtractPhaseImpl>;

}


