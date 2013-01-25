#include <Common/Global.h>
#include "Global.h"
#include "FontManager.h"
using namespace std;
using namespace Lightbox;

FontManager::FontManager(bool _canRender):
	m_canRender(_canRender)
{
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
