#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "ExtractPhase.h"
using namespace std;
using namespace lb;

void ExtractPhaseImpl::init()
{
}

void ExtractPhaseImpl::compute(std::vector<float>& _v)
{
	auto working = input.get();
	unsigned arity = working.size();
	_v.resize(arity / 2 + 1);

	for (unsigned i = 0; i <= arity / 2; i++)
	{
		float re = (i == arity / 2) ? 0 : working[i];
		float im = i ? working[arity - i] : 0;
		_v[i] = ::atan2(re, im) + pi<float>();
		while (_v[i] >= twoPi<float>())
			_v[i] -= twoPi<float>();
	}
}


