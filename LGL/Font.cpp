#include "FontFace.h"
#include "Font.h"

using namespace Lightbox;

Font::Font(uint8_t const* _ttfData, float _size):
	Pimpl(new FontFace(_ttfData, _size))
{}

void Font::draw(fCoord _anchor, std::string const& _text, Colour _c)
{
	if (m_p)
		m_p->draw(_anchor, _text, _c);
}

