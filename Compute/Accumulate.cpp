#include <Common/Global.h>
#include "Accumulate.h"
using namespace std;
using namespace lb;

void AccumulateImpl::compute(std::vector<float>& _v)
{
	unsigned h = input.info().size;
	if (m_last.size() != h * factor)
		m_last = std::vector<float>(h * factor, 0);

	valmove(m_last.data(), m_last.data() + h, h * (factor - 1));
	valcpy(m_last.data() + h * (factor - 1), input.get().data(), h);
	_v.resize(m_last.size());
	valcpy(_v.data(), m_last.data(), m_last.size());
}
