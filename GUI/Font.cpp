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

void Font::draw(fCoord _anchor, std::string const& _text, RGBA _c, AnchorType _t) const
{
	if (auto b = getBaked())
		b->draw(_anchor, _text, _c, _t);
}

void Font::draw(iCoord _anchor, std::string const& _text, RGBA _c, AnchorType _t) const
{
	if (auto b = getBaked())
		b->draw(fCoord(_anchor), _text, _c, _t);
}

fRect Font::measure(std::string const& _text, bool _tight) const
{
	return GUIApp::fontManager().getInfo(m_definition).measure(_text, m_mm, _tight);
}

fRect Font::measurePx(std::string const& _text, bool _tight) const
{
	return GUIApp::fontManager().getInfo(m_definition).measure(_text, GUIApp::joint().display->toUnalignedPixels(fSize(0, m_mm)).h(), _tight);
}
