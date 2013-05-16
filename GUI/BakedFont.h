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

struct stbtt_fontinfo;

namespace lb
{

class BakedFont;

class FontInfo
{
	friend class BakedFont;

public:
	FontInfo() {}
	FontInfo(foreign_vector<uint8_t const> const& _ttfData);

	fRect measure(std::string const& _text, float _size, bool _tight = false) const;
	CharMetrics metrics(Char _char, Char _nChar, float _size, bool _tight = false) const;

private:
	foreign_vector<uint8_t const> m_data;
	std::shared_ptr<stbtt_fontinfo> m_info;
};

class BakedFont
{
public:
	BakedFont(Font const& _f, FontInfo const& _info);
	~BakedFont();
	
	float pxSize() const { return m_pxSize; }

	void draw(fCoord _anchor, std::string const& _text, RGBA _c, AnchorType _anchorType);
	// deprecated - use Font's measurements instead.
	fRect pxMeasure(std::string const& _text) const;
	fRect measureMm(std::string const& _text) const;

private:
	Font m_f;
	FontInfo m_info;

	Program m_program;
	Attrib m_index;
	Attrib m_layout;
	Attrib m_source;
	Attrib m_size;
	Uniform m_color;
	Uniform m_tex;
	Uniform m_texSize;

	void* m_charData;
	Texture2D m_texture;
	Buffer<uint8_t> m_buffer;

	int m_above;
	int m_below;
	float m_pxSize;
};

typedef std::shared_ptr<BakedFont> BakedFontPtr;

}

