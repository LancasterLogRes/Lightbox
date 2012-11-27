#pragma once

#include <string>
#include <Common/Global.h>
#include "Global.h"
#include "Attrib.h"
#include "Program.h"
#include "Uniform.h"
#include "Texture2D.h"
#include "Buffer.h"

namespace Lightbox
{

class FontFace
{
public:
	FontFace(uint8_t const* _ttfData, float _size);

	void draw(iCoord _anchor, std::string const& _text, Colour _c);

	static void initForDisplay(uSize _s);

private:
	Attrib m_index;
	Attrib m_layout;
	Attrib m_source;
	Attrib m_size;
	Uniform m_color;
	Uniform m_tex;

	void* m_charData;
	Texture2D m_texture;
	Buffer<uint8_t> m_buffer;

	int m_above;
	int m_below;

	static Program s_program;
};

}
