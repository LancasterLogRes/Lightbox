#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class CycleDeltaImpl: public ComputeImplBase<float, SpectrumInfo>
{
public:
	CycleDeltaImpl(Compute<float, SpectrumInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~CycleDeltaImpl() {}
	
	virtual char const* name() const { return "CycleDelta"; }
	virtual SpectrumInfo info() { return input.info(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<float, SpectrumInfo> input;

private:
	std::vector<float> m_last;
};

using CycleDelta = ComputeBase<CycleDeltaImpl>;

}


