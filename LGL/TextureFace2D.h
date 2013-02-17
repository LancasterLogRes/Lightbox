#pragma once

#include <vector>
#include <Common/Global.h>
#include <Numeric/Coord.h>
#include "GL.h"
#include "Global.h"

namespace Lightbox
{

class TextureFace2D: public boost::noncopyable
{
public:
	TextureFace2D(): m_sampling(GL_LINEAR) { LB_GL(glGenTextures, 1, &m_id); }
	~TextureFace2D() { LB_GL(glDeleteTextures, 1, &m_id); }

	GLuint id() const { return m_id; }
	void bind() const { LB_GL(glBindTexture, GL_TEXTURE_2D, m_id); }
	iSize size() const { return m_dims; }

	void bindData(iSize const& _dims, foreign_vector<uint8_t> const& _data, GLenum _format, int _internalFormat, int _level);
	void framebufferColor(unsigned _index, unsigned _level) { LB_GL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + _index, GL_TEXTURE_2D, m_id, _level); }

	void activate(unsigned _unit) const;
	void deactivate(unsigned _unit) const;

	void setSampling(GLenum _s = GL_LINEAR) { m_sampling = _s; }

	void viewport() const { LB_GL(glViewport, 0, 0, m_dims.w(), m_dims.h()); }

private:
	iSize m_dims;
	GLuint m_id;
	GLenum m_sampling;
};

std::pair<iSize, foreign_vector<uint8_t> > readPng(std::function<void(uint8_t*, size_t)> const& _read);

}
