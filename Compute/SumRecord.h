#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class SumRecordImpl: public ComputeImplBase<ScalarInfo, float>
{
public:
	SumRecordImpl(Compute<VoidInfo, float> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~SumRecordImpl() {}
	
	virtual char const* name() const { return "SumRecord"; }
	virtual ScalarInfo info() { return ScalarInfo(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<VoidInfo, float> input;
};

using SumRecord = ComputeBase<SumRecordImpl>;

}


