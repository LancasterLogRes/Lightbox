#pragma once

#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class SumRecordImpl: public ComputeImplBase<float, ScalarInfo>
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

class MaxRecordImpl: public ComputeImplBase<float, ScalarInfo>
{
public:
	MaxRecordImpl(Compute<float, VoidInfo> const& _input): ComputeImplBase(input), input(_input) {}
	virtual ~MaxRecordImpl() {}

	virtual char const* name() const { return "MaxRecord"; }
	virtual ScalarInfo info() { return ScalarInfo(); }
	virtual void init();
	virtual void compute(std::vector<float>& _v);

	Compute<float, VoidInfo> input;
};

using MaxRecord = ComputeBase<MaxRecordImpl>;

}


