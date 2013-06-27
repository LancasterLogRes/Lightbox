#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "Bark.h"
using namespace std;
using namespace lb;

void BarkPhonImpl::init()
{
	// A bit ugly - we make an assumption over determining the original window size from the magnitude bands, but without general OOB dataflow, there's not much else to be done.
	m_bark.init(input.info().bands, toSeconds(input.info().windowDuration), s_barkBands);
	m_phon.init(s_barkCentres);
}

void BarkPhonImpl::compute(std::vector<float>& _v)
{
	auto in = input.get();
	_v.resize(26);
	for (unsigned cb = 0; cb < 26; ++cb)
		_v[cb] = m_phon.value(m_bark.mag(in, cb), cb);
}


