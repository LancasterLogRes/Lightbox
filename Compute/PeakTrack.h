#pragma once

#include <vector>
#include <Common/Peaks.h>
#include "Common.h"
#include "Compute.h"

namespace lb
{

struct PeakState: public FreqPeak
{
	float speed(float _gravity) const { return _gravity / mag; }
	void move(float _b, float _gravity)
	{
		band += sign(_b - band) * std::min<float>(speed(_gravity), fabs(_b - band));
		assert(band >= 0);
	}
	void attract(Peak<> _p, float _gravity)
	{
		float db = _p.band - band;
		band += sign(db) * std::min<float>(speed(_gravity) * _p.mag, fabs(db));
		assert(band > 0);
		mag += 1.f / (fabs(db) + 1) * (_p.mag - mag);
	}
	float fit(Peak<> _p) const { return fabs(_p.band - band) * fabs(_p.mag - mag); }
	bool operator<(PeakState _s) const { return band < _s.band; }
};

class PeakTrackImpl: public ComputeImplBase<SpectrumInfo, FreqPeak>
{
public:
	PeakTrackImpl(Compute<Peak<>, SpectrumInfo> const& _input, unsigned _count, float _gravity): ComputeImplBase(input, count, gravity), input(_input), count(_count), gravity(_gravity) {}
	virtual ~PeakTrackImpl() {}

	virtual char const* name() const { return "PeakTrackv2"; }
	virtual SpectrumInfo info() { return input.info(); }
	virtual void init();
	virtual void compute(std::vector<FreqPeak>& _v);

	Compute<Peak<>, SpectrumInfo> input;
	unsigned count;
	float gravity;

private:
	std::vector<PeakState> m_peaks;
};
using PeakTrack = ComputeBase<PeakTrackImpl>;

}


