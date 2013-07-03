#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class BarkPhonImpl: public ComputeImplBase<BarkPhonSpectrumInfo, float>
{
public:
	BarkPhonImpl(Compute<float, SpectrumInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~BarkPhonImpl() {}
	
	virtual char const* name() const { return "BarkPhon"; }
	virtual BarkPhonSpectrumInfo info() { return { 0, input.info().windowDuration, 26 }; }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<float, SpectrumInfo> input;

private:
	BandRemapper<float> m_bark;
	Phon<float> m_phon;
};

using BarkPhon = ComputeBase<BarkPhonImpl>;

}


