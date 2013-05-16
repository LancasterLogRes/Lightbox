#include "BakedFont.h"
#include "Font.h"
#include "GUIApp.h"
using namespace std;
using namespace lb;

Font::Font(Metric _m, float _u, FontDefinition const& _d):
	m_definition(_d),
	m_mm(_m == Metric::Millis ? _u : GUIApp::joint().display->fromPixels(fSize(0, _u)).h())
{
}

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

fRect Font::pxMeasure(std::string const& _text, bool _tight) const
{
	if (GUIApp::joint().display)
		return GUIApp::fontManager().getInfo(m_definition).measure(_text, pxSize(), _tight);
	else
		return fRect();
}

CharMetrics Font::metrics(Char _char, Char _nChar, bool _tight) const
{
	return GUIApp::fontManager().getInfo(m_definition).metrics(_char, _nChar, m_mm, _tight);
}

float Font::pxSize() const
{
	if (GUIApp::joint().display)
		return GUIApp::joint().display->toPixelsF(fSize(0, m_mm)).h();
	return 0.f;
}

CharMetrics Font::pxMetrics(Char _char, Char _nChar, bool _tight) const
{
	if (GUIApp::joint().display)
		return GUIApp::fontManager().getInfo(m_definition).metrics(_char, _nChar, pxSize(), _tight);
	else
		return CharMetrics();
}
