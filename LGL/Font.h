#pragma once

#include <Common/Pimpl.h>
#include <Common/Global.h>
#include <Numeric/Colour.h>
#include <Numeric/Coord.h>
#include "Global.h"

namespace Lightbox
{

class Font: Pimpl<FontFace>
{
public:
	Font() {}
	Font(uint8_t const* _ttfData, float _size);

	void draw(iCoord _anchor, std::string const& _text, Colour _c = Colour::Black);
};

}
