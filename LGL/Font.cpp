#include "Font.h"

using namespace Lightbox;

Font::Font(uint8_t const* _ttfData, float _size):
	Pimpl(new FontFace(_ttfData, _size))
{}

void Font::draw(float _x, float _y, std::string const& _text)
{
	if (m_p)
		m_p->draw(_x, _y, _text);
}

