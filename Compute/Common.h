#pragma once

#include <memory>
#include <Common/Global.h>
#include <Common/SimpleKey.h>
#include <Common/Time.h>
#include <Common/GraphMetadata.h>

namespace lb
{

struct VoidInfo {};

struct EventStreamInfo: public VoidInfo
{
};

struct ScalarInfo: public VoidInfo
{
	GraphMetadata::Axes axes() const { return { { "Value", XOf(), Range(0, 1) } }; }
};

struct PCMInfo: public ScalarInfo
{
	unsigned rate;
	unsigned size;
	PCMInfo() = default;
	PCMInfo(unsigned _r, unsigned _s): rate(_r), size(_s) {}
	GraphMetadata::Axes axes() const { return { { "Amplitude", XOf(), Range(0, 1) } }; }
};

struct SpectrumInfo: public VoidInfo
{
	unsigned nyquist;
	Time windowDuration;
	unsigned bands;
	SpectrumInfo() = default;
	SpectrumInfo(unsigned _n, Time _wD, unsigned _b): nyquist(_n), windowDuration(_wD), bands(_b) {}
	GraphMetadata::Axes axes() const { return { { "Frequency (Hz)", XOf(1 / toSeconds(windowDuration)), Range(0, bands / toSeconds(windowDuration)) }, { "Magnitude", XOf(), Range(0, 1) } }; }
};

struct BarkPhonSpectrumInfo: public SpectrumInfo
{
	BarkPhonSpectrumInfo() = default;
	BarkPhonSpectrumInfo(unsigned _n, Time _wD, unsigned _b): SpectrumInfo{_n, _wD, _b} {}
	GraphMetadata::Axes axes() const { return { { "Band (Bark)", XOf(), Range(0, 26) }, { "Phon", XOf(), Range(0, 100) } }; }
};

struct ComplexSpectrumInfo: public SpectrumInfo
{
	ComplexSpectrumInfo() = default;
	ComplexSpectrumInfo(unsigned _n, Time _wD, unsigned _b, bool _iHCP): SpectrumInfo{_n, _wD, _b}, isHCPacked(_iHCP) {}
	bool isHCPacked;
};

struct FreqPeak
{
	float band;
	float phase;
	float mag;
};

}
