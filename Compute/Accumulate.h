#pragma once

#include <Common/Algorithms.h>
#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class AccumulateImpl: public ComputeImpl<PCMInfo, float>
{
public:
	AccumulateImpl(Compute<PCMInfo, float> const& _input, unsigned _factor = 8): input(_input), factor(_factor) {}
	virtual SimpleKey hash() { return generateKey("Accumulate", input.hash(), factor); }
	virtual PCMInfo info()
	{
		return { input.info().rate, input.info().size * factor };
	}
	virtual void init()
	{
		m_last = std::vector<float>(input.info().size * factor, 0);
	}
	virtual void compute(std::vector<float>& _v);

	Compute<PCMInfo, float> input;
	unsigned factor;

private:
	std::vector<float> m_last;
};
using Accumulate = ComputeBase<AccumulateImpl>;

}


