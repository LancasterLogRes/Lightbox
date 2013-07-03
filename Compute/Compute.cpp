#include <Common/Global.h>
#include "Compute.h"
using namespace std;
using namespace lb;

thread_local ComputeRegistrar* lb::ComputeRegistrar::s_this = nullptr;

void ComputeRegistrar::init()
{
	cdebug << "ComputeReg::init";
	m_memos.clear();
	onInit();
}

void ComputeRegistrar::beginTime(lb::Time _t)
{
	for (auto& i: m_memos)
		i.second.second.reset();
	onBeginTime(_t);
	m_time = _t;
}

lb::foreign_vector<uint8_t> ComputeRegistrar::compute(GenericComputeImpl* _c)
{
	SimpleKey h = _c->hash();
	auto it = m_memos.find(h);
	if (it == m_memos.end())
	{
		cdebug << "ComputeReg: Initializing compute" << _c->name() << hex << h;
		_c->init();
		bool b;
		tie(it, b) = m_memos.insert(std::make_pair(h, std::pair<std::vector<uint8_t>, lb::foreign_vector<uint8_t> >()));
	}

	lb::foreign_vector<uint8_t> ret = it->second.second;
	if (!ret.size())
	{
		// Memo hasn't been accessed yet this time...
		insertMemo(h);
		ret = it->second.second;
	}
	if (!ret.size())
	{
		// Memo not yet computed...
		std::vector<uint8_t>& vbyte = it->second.first;
		_c->genericCompute(vbyte);
		ret = it->second.second = lb::foreign_vector<uint8_t>(&vbyte);
	}

	return ret;
}
