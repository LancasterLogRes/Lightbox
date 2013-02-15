#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "BakedFont.h"
#include "GUIApp.h"
using namespace std;
using namespace Lightbox;

static const unsigned s_maxAtOnce = 64;
static const unsigned s_charDataFirst = 32;
static const unsigned s_charDataCount = 96;

FontInfo::FontInfo(foreign_vector<uint8_t const> const& _ttfData):
	m_data(_ttfData)
{
	m_info = shared_ptr<stbtt_fontinfo>(new struct stbtt_fontinfo);
	stbtt_InitFont(&*m_info, m_data.data(), 0);
}

fRect FontInfo::measure(std::string const& _text, float _size, bool _tight) const
{
	if (!m_info)
		return fRect();

	// Keep everything as non-units for now - might be wrong if it does clever pixel hinting, but probably not very...
	float scale = stbtt_ScaleForMappingEmToPixels(&*m_info, _size);
	int ascent;
	int descent;
	int lineGap;
	stbtt_GetFontVMetrics(&*m_info, &ascent, &descent, &lineGap);

	iRect box;
	iCoord cursor;
	for (unsigned i = 0; i < _text.size(); ++i)
	{
		char c = _text[i];
		if (c == '\n')
			cursor.set(0, cursor.y() + ascent - descent + lineGap);
		else
		{
			int advance;
			int leftSideBearing;
			int fx;
			int tx;
			int fy;
			int ty;
			stbtt_GetCodepointBox(&*m_info, c, &fx, &fy, &tx, &ty);
			if (!_tight)
			{
				fy = descent;
				ty = ascent;
			}
			stbtt_GetCodepointHMetrics(&*m_info, c, &advance, &leftSideBearing);
			box.include(cursor + iCoord(fx, -fy));
			box.include(cursor + iCoord(tx, -ty));
			cursor += iSize(advance, 0);
			if ((i + 1) < _text.size())
				cursor += iSize(stbtt_GetCodepointKernAdvance(&*m_info, c, (&c)[1]), 0);
		}
	}
	return fRect(box) * scale;
}

BakedFont::BakedFont(Font const& _f, FontInfo const& _info): m_f(_f), m_info(_info)
{
	float pxSize(GUIApp::joint().display->toUnalignedPixels(fSize(0, _f.mmSize())).height());

	m_program = Program("Shaders.glsl", "font");
	m_program.tie(GUIApp::joint().uniforms);

	m_index = m_program.attrib("a_index");
	m_layout = m_program.attrib("a_layout");
	m_source = m_program.attrib("a_source");
	m_size = m_program.attrib("a_size");
	m_color = m_program.uniform("u_color");
	m_tex = m_program.uniform("u_tex");
	m_texSize = m_program.uniform("u_texSize");

	m_charData = new stbtt_bakedchar[s_charDataCount];
	uSize bitmap(max<int>(32, pxSize * 4), max<int>(512, pxSize * 4));
	unsigned char* tempBitmap;
	while (true)
	{
		tempBitmap = new unsigned char[bitmap.w()*bitmap.h()];
		int by = stbtt_BakeFontBitmap(_info.m_data.data(), 0, stbtt_ScaleForMappingEmToPixels(&*(_info.m_info), pxSize) / stbtt_ScaleForPixelHeight(&*(_info.m_info), pxSize) * pxSize, tempBitmap, bitmap.w(), bitmap.h(), s_charDataFirst, s_charDataCount, (stbtt_bakedchar*)m_charData);
		if (by > 0)
			break;
		delete [] tempBitmap;
		bitmap.setHeight(bitmap.height() * 2);
	}

	m_above = 0;
	m_below = 0;
	for (unsigned i = 0; i < s_charDataCount; ++i)
	{
		m_above = max<int>(m_above, -((stbtt_bakedchar*)m_charData)[i].yoff);
		m_below = max<int>(m_below, (((stbtt_bakedchar*)m_charData)[i].y1 - ((stbtt_bakedchar*)m_charData)[i].y0) + ((stbtt_bakedchar*)m_charData)[i].yoff);
	}
	m_texture = Texture2D(bitmap, foreign(tempBitmap, sizeof(tempBitmap)), GL_ALPHA, GL_ALPHA);
	delete [] tempBitmap;

	array<uint8_t, 12> corners = {{ 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1 }};
	m_buffer = Buffer<uint8_t>(s_maxAtOnce * corners.size());
	for (unsigned i = 0; i < s_maxAtOnce; ++i)
		m_buffer.set(corners.data(), corners.size(), i * corners.size());
}

BakedFont::~BakedFont()
{
}

void BakedFont::draw(fCoord _anchor, string const& _text, RGBA _c, AnchorType _t)
{
	if (_text == "")
		return;

	assert(m_program.isValid());

	std::vector<std::array<GLshort, 2> > layout(_text.size() * 6);
	std::vector<std::array<GLushort, 2> > source(_text.size() * 6);
	std::vector<std::array<GLubyte, 2> > size(_text.size() * 6);

	float width = 0;
	for (uint8_t c: _text)
		if (c >= (int)s_charDataFirst && c < int(s_charDataFirst + s_charDataCount))
			width += ((stbtt_bakedchar*)m_charData)[c - s_charDataFirst].xadvance;

	float pxSize(GUIApp::joint().display->toUnalignedPixels(fSize(0, m_f.mmSize())).height());
	fRect bb = m_info.measure(_text, pxSize, _t & TightVertical);

	float x = _t & AtLeft ? _anchor.x() - bb.left() : _t & AtRight ? _anchor.x() - bb.right() : (_anchor.x() - bb.middle().x());
	float y = _t & AtTop ? _anchor.y() - bb.top() : _t & AtBottom ? _anchor.y() - bb.bottom() : _t & AtBaseline ? _anchor.y() : (_anchor.y() - bb.middle().y());

	for (unsigned i = 0; i < _text.size(); ++i)
	{
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad((stbtt_bakedchar*)m_charData, 512, 512, _text[i] - s_charDataFirst, &x, &y, &q, 1);
		for (int j = 0; j < 6; ++j)
			layout[i * 6 + j] = {{(GLshort)q.x0, (GLshort)q.y0}},
			source[i * 6 + j] = {{(GLushort)(q.s0 * 512), (GLushort)(q.t0 * 512)}},
			size[i * 6 + j] = {{GLubyte(q.x1 - q.x0), GLubyte(q.y1 - q.y0)}};
	}

	ProgramUser u(m_program);
	m_color = _c;
	m_tex = m_texture;
	m_texSize = fVector2(m_texture.size().w(), m_texture.size().h());
	m_index.setData(m_buffer, 2);
	for (int off = 0; off < (int)_text.size(); off += s_maxAtOnce)
	{
		m_layout.setStaticData(layout.data() + off * 6, 2);
		m_source.setStaticData(source.data() + off * 6, 2);
		m_size.setStaticData(size.data() + off * 6, 2);
		u.triangles(min<int>(_text.size() - off, s_maxAtOnce) * 6);
	}
}

fRect BakedFont::measurePx(std::string const& _text) const
{
	float width = 0;
	for (char c: _text)
		width += ((stbtt_bakedchar*)m_charData)[c - s_charDataFirst].xadvance;
	return fRect(fCoord(0, -m_above), fSize(width, m_above + m_below + 1));
}

fRect BakedFont::measureMm(std::string const& _text) const
{
	return measurePx(_text) * float(GUIApp::joint().displaySizeMM.height() / GUIApp::joint().displaySizePixels.height());
}
