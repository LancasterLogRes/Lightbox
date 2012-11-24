#pragma once

#include <Common/Pimpl.h>
#include <Common/Global.h>
#include "Global.h"

namespace Lightbox
{

class Font: Pimpl<FontFace>
{
public:
	Font() {}
	Font(uint8_t const* _ttfData, float _size);

	void draw(float _x, float _y, std::string const& _text);
};

}
