#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class MeanRecordImpl: public ComputeImplBase<ScalarInfo, float>
{
public:
	MeanRecordImpl(Compute<VoidInfo, float> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~MeanRecordImpl() {}
	
	virtual char const* name() const { return "MeanRecord"; }
	virtual ScalarInfo info() { return ScalarInfo(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<VoidInfo, float> input;
};

using MeanRecord = ComputeBase<MeanRecordImpl>;

}


