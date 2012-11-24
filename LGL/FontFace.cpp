#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "FontFace.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

static const unsigned s_maxAtOnce = 64;
static const unsigned s_charDataFirst = 32;
static const unsigned s_charDataCount = 96;

FontFace::FontFace(uint8_t const* _ttfData, float _size)
{
	m_program = Program(Shader::vertex(LB_R(Font_vert)), Shader::fragment(LB_R(Font_frag)));

	m_index = m_program.attrib("a_index");
	m_layout = m_program.attrib("a_layout");
	m_source = m_program.attrib("a_source");
	m_size = m_program.attrib("a_size");
	m_tex = m_program.uniform("u_tex");

	unsigned char temp_bitmap[512*512];

	m_charData = new stbtt_bakedchar[s_charDataCount];
	stbtt_BakeFontBitmap(_ttfData,0, _size, temp_bitmap,512,512, s_charDataFirst,s_charDataCount, (stbtt_bakedchar*)m_charData);
	m_texture = Texture2D(uSize(512, 512), foreign(temp_bitmap, sizeof(temp_bitmap)), GL_ALPHA, GL_ALPHA);

	array<uint8_t, 12> corners = {{ 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1 }};
	m_buffer = Buffer<uint8_t>(s_maxAtOnce * corners.size());
	for (unsigned i = 0; i < s_maxAtOnce; ++i)
		m_buffer.set(corners.data(), corners.size(), i * corners.size());
}

void FontFace::draw(float _x, float _y, string const& _text)
{
	std::vector<std::array<GLshort, 2> > layout(_text.size() * 6);
	std::vector<std::array<GLushort, 2> > source(_text.size() * 6);
	std::vector<std::array<GLubyte, 2> > size(_text.size() * 6);
	for (unsigned i = 0; i < _text.size(); ++i)
	{
		stbtt_aligned_quad q;
		stbtt_GetBakedQuad((stbtt_bakedchar*)m_charData, 512, 512, _text[i] - s_charDataFirst, &_x, &_y, &q, 1);
		for (int j = 0; j < 6; ++j)
			layout[i * 6 + j] = {{(GLshort)q.x0, (GLshort)q.y0}},
			source[i * 6 + j] = {{GLshort(q.s0 * 512), GLshort(q.t0 * 512)}},
			size[i * 6 + j] = {{GLubyte(q.x1 - q.x0), GLubyte(q.y1 - q.y0)}};
	}

	ProgramUser u(m_program);
	m_tex = m_texture;
	m_index.setData(m_buffer, 2);
	m_layout.setStaticData(layout.data(), 2);
	m_source.setStaticData(source.data(), 2);
	m_size.setStaticData(size.data(), 2);
	u.triangles(_text.size() * 6);
}
