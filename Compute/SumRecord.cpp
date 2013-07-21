#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "SumRecord.h"
using namespace std;
using namespace lb;

void SumRecordImpl::init()
{
}

void SumRecordImpl::compute(std::vector<float>& _v)
{
	_v.resize(1);
	_v[0] = 0;
	for (auto i: input.get())
		_v[0] += i;
}



void MaxRecordImpl::init()
{
}

void MaxRecordImpl::compute(std::vector<float>& _v)
{
	_v.resize(1);
	auto in = input.get();
	_v[0] = in[0];
	for (auto i: in)
		_v[0] = max(_v[0], i);
}


