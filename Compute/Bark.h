#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class BarkImpl: public ComputeImplBase<BarkSpectrumInfo, float>
{
public:
	BarkImpl(Compute<SpectrumInfo, float> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~BarkImpl() {}
	
	virtual char const* name() const { return "Bark"; }
	virtual BarkSpectrumInfo info() { return { 0, input.info().windowDuration, 26 }; }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<SpectrumInfo, float> input;

private:
	BandRemapper<float> m_bark;
	Phon<float> m_phon;
};

using Bark = ComputeBase<BarkImpl>;

}


