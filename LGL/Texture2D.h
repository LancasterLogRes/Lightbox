#pragma once

#include <boost/noncopyable.hpp>
#include <Common/Pimpl.h>
#include "TextureFace2D.h"

namespace Lightbox
{

class TextureActivator;

class Texture2D: public Pimpl<TextureFace2D>
{
public:
	Texture2D() {}
	Texture2D(uSize const& _dims, foreign_vector<uint8_t> const& _data = foreign_vector<uint8_t>(), GLenum _format = GL_RGBA, int _internalFormat = GL_RGBA, int _level = 0): Pimpl<TextureFace2D>(new TextureFace2D) { m_p->bindData(_dims, _data, _format, _internalFormat, _level); }
	explicit Texture2D(std::pair<uSize, foreign_vector<uint8_t> > const& _data, GLenum _format = GL_RGBA, int _internalFormat = GL_RGBA, int _level = 0): Pimpl<TextureFace2D>(new TextureFace2D) { m_p->bindData(_data.first, _data.second, _format, _internalFormat, _level); }

	uSize size() const { return m_p ? m_p->size() : uSize(0, 0); }
	void activate(uint _unit) const { if (m_p) m_p->activate(_unit); }
	void deactivate(uint _unit) const { if (m_p) m_p->deactivate(_unit); }

	void framebufferColor(unsigned _index = 0, unsigned _level = 0) { if (m_p) m_p->framebufferColor(_index, _level); }

	typedef TextureActivator Activator;
};

class TextureActivator: public boost::noncopyable
{
public:
	TextureActivator(Texture2D const& _t, uint _unit): m_t(_t), m_unit(_unit) { _t.activate(_unit); }
	~TextureActivator() { m_t.deactivate(m_unit); }
	Texture2D m_t;
	uint m_unit;
};

}
