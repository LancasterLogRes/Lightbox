#include <Common/Global.h>
#include <boost/algorithm/string/predicate.hpp>
#include "Global.h"
#include "FontManager.h"
using namespace std;
using namespace lb;

FontManager::FontManager(bool _canRender):
	m_canRender(_canRender)
{
	// Register all .ttf files
	for (auto const& r: Resources::map())
		if (boost::algorithm::ends_with(boost::algorithm::to_lower_copy(r.first), ".ttf"))
			if (r.first[r.first.size() - 6] == '-' || r.first[r.first.size() - 6] == '_')
				if (tolower(r.first[r.first.size() - 5]) == 'r')
					registerData(FontDefinition(r.first.substr(0, r.first.size() - 6), false), r.second);
				else if (tolower(r.first[r.first.size() - 5]) == 'b')
					registerData(FontDefinition(r.first.substr(0, r.first.size() - 6), true), r.second);
				else
					registerData(FontDefinition(r.first.substr(0, r.first.size() - 4), false), r.second);
			else
				registerData(FontDefinition(r.first.substr(0, r.first.size() - 4), false), r.second);
		else {}
}

FontManager::~FontManager()
{
}

BakedFontPtr FontManager::getBaked(Font const& _f)
{
	if (m_canRender)
	{
		if (m_rendered.count(_f))
			return m_rendered.at(_f);
		if (m_info.count(_f.definition()))
			return (m_rendered[_f] = BakedFontPtr(new BakedFont(_f, m_info.at(_f.definition()))));
	}
	return BakedFontPtr();
}
