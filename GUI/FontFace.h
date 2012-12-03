#pragma once

#include <string>
#include <Common/Global.h>
#include <Common/RGBA.h>
#include <LGL/Attrib.h>
#include <LGL/Program.h>
#include <LGL/Uniform.h>
#include <LGL/Texture2D.h>
#include <LGL/Buffer.h>
#include "Global.h"

namespace Lightbox
{

class FontFace
{
public:
	FontFace(uint8_t const* _ttfData, float _size);

	fSize measure(std::string const& _text) const;
	void draw(fCoord _anchor, std::string const& _text, RGBA _c);

	static void initForDisplay(uSize _s);

private:
	Program m_program;
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
};

}
