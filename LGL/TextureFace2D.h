#pragma once

#include <vector>
#include <GLES2/gl2.h>
#include <Numeric/Rect.h>
#include <Common/Global.h>
#include "Global.h"

namespace Lightbox
{

class TextureFace2D: public boost::noncopyable
{
public:
	TextureFace2D() { LIGHTBOX_GL(glGenTextures(1, &m_id)); }
	~TextureFace2D() { LIGHTBOX_GL(glDeleteTextures(1, &m_id)); }

	GLuint id() const { return m_id; }
	void bind() const { LIGHTBOX_GL(glBindTexture(GL_TEXTURE_2D, m_id)); }

	void bindData(uSize const& _dims, foreign_vector<uint8_t> const& _data, GLenum _format, int _internalFormat, int _level);

	void activate(unsigned _unit) const;
	void deactivate(unsigned _unit) const;

private:
	GLuint m_id;
};

std::pair<uSize, foreign_vector<uint8_t> > readPng(std::function<void(uint8_t*, size_t)> const& _read);

}
