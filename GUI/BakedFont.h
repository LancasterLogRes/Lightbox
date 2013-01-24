#pragma once

#include <string>
#include <memory>
#include <Common/Global.h>
#include <Common/RGBA.h>
#include <LGL/Attrib.h>
#include <LGL/Program.h>
#include <LGL/Uniform.h>
#include <LGL/Texture2D.h>
#include <LGL/Buffer.h>
#include "Global.h"
#include "Font.h"

namespace Lightbox
{

class BakedFont
{
public:
	BakedFont(Font const& _f, uint8_t const* _ttfData);
	~BakedFont();
	
	fSize measure(std::string const& _text) const;
	void draw(fCoord _anchor, std::string const& _text, RGBA _c);

private:
	Font m_f;
	float m_pxSize;

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

typedef std::shared_ptr<BakedFont> BakedFontPtr;

}

