#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "ExtractMagnitude.h"
using namespace std;
using namespace lb;

void ExtractMagnitudeImpl::init()
{
}

void ExtractMagnitudeImpl::compute(std::vector<float>& _v)
{
	auto working = input.get();
	unsigned arity = working.size();
	_v.resize(arity / 2 + 1);

	for (unsigned i = 0; i <= arity / 2; i++)
	{
		float re = (i == arity / 2) ? 0 : working[i];
		float im = i ? working[arity - i] : 0;
		float p = (re * re + im * im) / float(arity);
		float m = (isFinite(p) && p != 0.f) ? sqrt(p) : 0;
		_v[i] = m;
	}
}
