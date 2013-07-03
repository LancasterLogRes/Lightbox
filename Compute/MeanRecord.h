#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class MeanRecordImpl: public ComputeImplBase<ScalarInfo, float>
{
public:
	MeanRecordImpl(Compute<float, VoidInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~MeanRecordImpl() {}
	
	virtual char const* name() const { return "MeanRecord"; }
	virtual ScalarInfo info() { return ScalarInfo(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<float, VoidInfo> input;
};

using MeanRecord = ComputeBase<MeanRecordImpl>;

}


