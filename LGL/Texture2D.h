#pragma once

#include <boost/noncopyable.hpp>
#include <Common/Pimpl.h>
#include "TextureFace2D.h"

namespace Lightbox
{

class TextureActivator;
class Program;

class Texture2D: public Pimpl<TextureFace2D>
{
public:
	Texture2D() {}
	Texture2D(iSize const& _dims, foreign_vector<uint8_t> const& _data = foreign_vector<uint8_t>(), GLenum _format = GL_RGBA, int _internalFormat = GL_RGBA, int _level = 0): Pimpl<TextureFace2D>(new TextureFace2D) { m_p->bindData(_dims, _data, _format, _internalFormat, _level); }
	explicit Texture2D(std::pair<iSize, foreign_vector<uint8_t> > const& _data, GLenum _format = GL_RGBA, int _internalFormat = GL_RGBA, int _level = 0): Pimpl<TextureFace2D>(new TextureFace2D) { m_p->bindData(_data.first, _data.second, _format, _internalFormat, _level); }

	iSize size() const { return m_p ? m_p->size() : iSize(0, 0); }
	void activate(uint _unit) const { if (m_p) m_p->activate(_unit); }
	void deactivate(uint _unit) const { if (m_p) m_p->deactivate(_unit); }

	void setSampling(GLenum _sampling = GL_LINEAR) { if (m_p) m_p->setSampling(_sampling); }

	void framebufferColor(unsigned _index = 0, unsigned _level = 0) { if (m_p) m_p->framebufferColor(_index, _level); }
	void viewport() { if (m_p) m_p->viewport(); }

	Texture2D filter(Program const& _p, Texture2D _out = Texture2D()) const;

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
