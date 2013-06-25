#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "ZeroCrossings.h"
using namespace std;
using namespace lb;

void ZeroCrossingsImpl::init()
{
}

void ZeroCrossingsImpl::compute(std::vector<float>& _v)
{
	_v.resize(1);
	auto vs = input.get();
	int zeroXs = 0;	// even->-ve, odd->+ve
	for (auto v: vs)
		if ((v > 0) == !(zeroXs % 2))
			++zeroXs;
	_v[0] = (zeroXs - (vs[0] > 0 ? 1 : 0)) / float(vs.size());
}


