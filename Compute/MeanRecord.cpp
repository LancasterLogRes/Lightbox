#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "MeanRecord.h"
using namespace std;
using namespace lb;

void MeanRecordImpl::init()
{
}

void MeanRecordImpl::compute(std::vector<float>& _v)
{
	_v.resize(1);
	_v[0] = 0;
	auto in = input.get();
	for (auto i: in)
		_v[0] += i;
	_v[0] /= in.size();
}


