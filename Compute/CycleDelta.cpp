#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "CycleDelta.h"
using namespace std;
using namespace lb;

void CycleDeltaImpl::init()
{
}

void CycleDeltaImpl::compute(std::vector<float>& _v)
{
	auto c = input.get();

	auto s = c.size();
	if (m_last.size() == s)
	{
		_v.resize(s);
		unsigned i = 0;
		for (; i < s - 3; i+=4)
		{
			float r0 = fabs(c[i] - m_last[i]);
			_v[i] = min(r0, fTwoPi - r0);
			float r1 = fabs(c[i+1] - m_last[i+1]);
			_v[i+1] = min(r1, fTwoPi - r1);
			float r2 = fabs(c[i+2] - m_last[i+2]);
			_v[i+2] = min(r2, fTwoPi - r2);
			float r3 = fabs(c[i+3] - m_last[i+3]);
			_v[i+3] = min(r3, fTwoPi - r3);
		}
		for (; i < s; ++i)
		{
			float r = fabs(c[i] - m_last[i]);
			_v[i] = min(r, twoPi<float>() - r);
		}
	}

	m_last.resize(c.size());
	valcpy(m_last.data(), c.data(), c.size());
}


