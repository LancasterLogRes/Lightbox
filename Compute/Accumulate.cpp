#include <Common/Global.h>
#include "Accumulate.h"
using namespace std;
using namespace lb;

void AccumulateWaveImpl::compute(std::vector<float>& _v)
{
	unsigned h = input.info().size;
	if (m_last.size() != h * factor)
		m_last = std::vector<float>(h * factor, 0);

	valmove(m_last.data(), m_last.data() + h, h * (factor - 1));
	valcpy(m_last.data() + h * (factor - 1), input.get().data(), h);
	_v.resize(m_last.size());
	valcpy(_v.data(), m_last.data(), m_last.size());


}

void ExtractChannelImpl::compute(std::vector<float>& _v)
{
	MultiPCMInfo iinfo = input.info();
	_v.resize(iinfo.size);
	auto i = input.get();
	valcpy(_v.data(), i.data() + channel, _v.size(), 1, iinfo.channels);
}

void MixChannelsImpl::compute(std::vector<float>& _v)
{
	MultiPCMInfo iinfo = input.info();
	_v.resize(iinfo.size);
	auto in = input.get();
	unsigned cs = min<unsigned>(levels.size(), iinfo.channels);
	for (unsigned i = 0; i < iinfo.size; ++i)
	{
		_v[i] = 0;
		for (unsigned c = 0; c < cs; ++c)
			_v[i] += levels[c] * in[i * iinfo.channels + c];
	}
}
