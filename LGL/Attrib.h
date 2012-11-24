#pragma once

#include <memory>
#include <Numeric.h>
#include <GLES2/gl2.h>
#include "Buffer.h"
#include "Global.h"

namespace Lightbox
{

class Attrib
{
public:
	Attrib(): m_location(0) {}
	Attrib(Program const& _p, std::string const& _name);

	GLint location() const { return m_location; }

	template <class _T> void setStaticData(_T const* _data, unsigned _size = 1, unsigned _stride = 0) { glBindBuffer(GL_ARRAY_BUFFER, 0); LIGHTBOX_GL(glVertexAttribPointer(m_location, _size, GLTypeTraits<_T>::typeEnum, GL_FALSE, _stride, _data)); }
	template <class _T> void setData(Buffer<_T> const& _b, unsigned _size = 1, unsigned _stride = 0, unsigned _offset = 0) { _b.sharedPtr()->bind(); LIGHTBOX_GL(glVertexAttribPointer(m_location, _size, GLTypeTraits<_T>::typeEnum, GL_FALSE, _stride, (GLvoid*)_offset)); }

	void enable() { LIGHTBOX_GL(glEnableVertexAttribArray(m_location)); }
	void disable() { LIGHTBOX_GL(glDisableVertexAttribArray(m_location)); }

private:
	std::weak_ptr<ProgramFace> m_p;
	GLint m_location;
};

}
