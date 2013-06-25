#pragma once

#include <vector>
#include <Common/FFT.h>
#include "Common.h"
#include "Compute.h"

namespace lb
{

class WindowedFourierImpl: public ComputeImplBase<ComplexSpectrumInfo, float>
{
public:
	WindowedFourierImpl(Compute<PCMInfo, float> const& _input, WindowFunction _function = HannWindow, float _functionParam = 0, bool _zeroPhase = true): ComputeImplBase(input, function, functionParam, zeroPhase), input(_input), function(_function), functionParam(_functionParam), zeroPhase(_zeroPhase) {}
	virtual ~WindowedFourierImpl() {}
	
	virtual char const* name() const { return "WindowedFourier"; }
	virtual ComplexSpectrumInfo info() { return { input.info().rate / 2, toBase(input.info().size, input.info().rate), input.info().size / 2 + 1, true }; }
	virtual void init();
	virtual void compute(std::vector<float>& _v);
	
	Compute<PCMInfo, float> input;
	WindowFunction function;
	float functionParam;
	bool zeroPhase;

private:
	std::vector<float> m_window;
	FFT<float> m_fft;
};

using WindowedFourier = ComputeBase<WindowedFourierImpl>;

}


