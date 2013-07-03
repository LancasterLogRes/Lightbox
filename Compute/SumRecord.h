#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class SumRecordImpl: public ComputeImplBase<ScalarInfo, float>
{
public:
	SumRecordImpl(Compute<float, VoidInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~SumRecordImpl() {}
	
	virtual char const* name() const { return "SumRecord"; }
	virtual ScalarInfo info() { return ScalarInfo(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<float, VoidInfo> input;
};

using SumRecord = ComputeBase<SumRecordImpl>;

}


