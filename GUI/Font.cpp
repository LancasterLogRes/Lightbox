#include "BakedFont.h"
#include "Font.h"
#include "GUIApp.h"
using namespace std;
using namespace Lightbox;

BakedFontPtr Font::getBaked() const
{
	if (BakedFontPtr r = m_baked.lock())
		return r;
	else
	{
		BakedFontPtr ret = GUIApp::fontManager().getBaked(*this);
		m_baked = ret;
		return ret;
	}
}

void Font::draw(fCoord _anchor, std::string const& _text, RGBA _c) const
{
	if (auto b = getBaked())
		b->draw(_anchor, _text, _c);
}

void Font::draw(iCoord _anchor, std::string const& _text, RGBA _c) const
{
	if (auto b = getBaked())
		b->draw(fCoord(_anchor), _text, _c);
}

fSize Font::measure(std::string const& _text) const
{
	if (auto b = getBaked())
		return b->measure(_text);
	else
		return fSize(0, 0);
}

iSize Font::measurePx(std::string const& _text) const
{
	if (auto b = getBaked())
		return (iSize)b->measure(_text);
	else
		return iSize(0, 0);
}
