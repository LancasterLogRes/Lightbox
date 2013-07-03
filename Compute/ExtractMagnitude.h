#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class ExtractMagnitudeImpl: public ComputeImplBase<SpectrumInfo, float>
{
public:
	ExtractMagnitudeImpl(Compute<float, ComplexSpectrumInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~ExtractMagnitudeImpl() {}
	
	virtual char const* name() const { return "ExtractMagnitude"; }
	virtual SpectrumInfo info() { return input.info(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<float, ComplexSpectrumInfo> input;
};

using ExtractMagnitude = ComputeBase<ExtractMagnitudeImpl>;

}


