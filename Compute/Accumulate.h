#pragma once

#include <Common/Algorithms.h>
#include <vector>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class AccumulateWaveImpl: public ComputeImplBase<float, PCMInfo>
{
public:
	AccumulateWaveImpl(Compute<float, PCMInfo> const& _input, unsigned _factor = 8): ComputeImplBase(input, factor), input(_input), factor(_factor) {}
	virtual char const* name() const { return "Accumulate"; }
	virtual PCMInfo info()
	{
		return { input.info().rate, input.info().size * factor };
	}
	virtual void init()
	{
		m_last = std::vector<float>(input.info().size * factor, 0);
	}
	virtual void compute(std::vector<float>& _v);

	Compute<float, PCMInfo> input;
	unsigned factor;

private:
	std::vector<float> m_last;
};
using AccumulateWave = ComputeBase<AccumulateWaveImpl>;

class ExtractChannelImpl: public ComputeImplBase<float, PCMInfo>
{
public:
	ExtractChannelImpl(Compute<float, MultiPCMInfo> const& _input, unsigned _channel = 0): ComputeImplBase(input, channel), input(_input), channel(_channel) {}
	virtual char const* name() const { return "ExtractChannel"; }
	virtual PCMInfo info() { return { input.info().rate, input.info().size }; }
	virtual void init() {}
	virtual void compute(std::vector<float>& _v);

	Compute<float, MultiPCMInfo> input;
	unsigned channel;
};
using ExtractChannel = ComputeBase<ExtractChannelImpl>;

class MixChannelsImpl: public ComputeImplBase<float, PCMInfo>
{
public:
	MixChannelsImpl(Compute<float, MultiPCMInfo> const& _input, std::vector<float> _levels = {{ 1 }}): ComputeImplBase(input), input(_input), levels(_levels) {}
	virtual bool isVolatile() const { return true; }
	virtual char const* name() const { return "MixChannels"; }
	virtual PCMInfo info() { return { input.info().rate, input.info().size }; }
	virtual void init() {}
	virtual void compute(std::vector<float>& _v);

	Compute<float, MultiPCMInfo> input;
	std::vector<float> levels;
};
using MixChannels = ComputeBase<MixChannelsImpl>;

}


