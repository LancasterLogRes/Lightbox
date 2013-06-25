#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "WindowedFourier.h"
using namespace std;
using namespace lb;

void WindowedFourierImpl::init()
{
	m_window = windowFunction(input.info().size, function, functionParam);
	m_fft.setArity(m_window.size());
}

void WindowedFourierImpl::compute(std::vector<float>& _v)
{
	auto in = input.get();
	_v.resize(in.size());
	// Prepare waveform for FFT...
	unsigned accOffset = zeroPhase ? m_window.size() / 2 : 0;
	// First half (up to accOffset).
	auto w = m_window.data() + accOffset;
	auto a = in.data() + accOffset;
	auto d = m_fft.in();
	for (auto de = m_fft.in() + accOffset; d != de; ++d, ++w, ++a)
		*d = *w * *a;
	// Second half (after accOffset).
	w = m_window.data();
	a = in.data();
	for (auto de = m_fft.in() + m_window.size(); d != de; ++d, ++w, ++a)
		*d = *w * *a;
	m_fft.rawProcess();
	valcpy(_v.data(), m_fft.out(), m_fft.arity());

	// Only got complex series - not yet split into mag/phase.
}
