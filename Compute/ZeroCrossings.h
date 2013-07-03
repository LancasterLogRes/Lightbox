#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class ZeroCrossingsImpl: public ComputeImplBase<ScalarInfo, float>
{
public:
	ZeroCrossingsImpl(Compute<float, PCMInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~ZeroCrossingsImpl() {}
	
	virtual char const* name() const final override { return "ZeroCrossings"; }
	virtual ScalarInfo info() { return {}; }
	virtual void init();
	virtual void compute(std::vector<float>& _v);

	Compute<float, PCMInfo> input;
};

using ZeroCrossings = ComputeBase<ZeroCrossingsImpl>;

}


