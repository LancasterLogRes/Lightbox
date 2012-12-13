#pragma once

#include <Common/Pimpl.h>
#include <Common/Global.h>
#include <Common/RGBA.h>
#include <Numeric/Coord.h>
#include "Global.h"

namespace Lightbox
{

class FontFace;

class Font: public Pimpl<FontFace>
{
public:
	Font() {}
	Font(uint8_t const* _ttfData, float _size);

	void draw(fCoord _anchor, std::string const& _text, RGBA _c = RGBA::Black) const;
	fSize measure(std::string const& _text) const;
};

}
