#pragma once

#include <memory>
#include <Numeric.h>
#include "GL.h"
#include "Buffer.h"
#include "Global.h"

namespace Lightbox
{

class Attrib
{
public:
	Attrib(): m_location(0) {}
	Attrib(Program const& _p, std::string const& _name);

	template <class _T> void setStaticData(_T const* _data, unsigned _size = 1, unsigned _stride = 0) const { if (Assert(isActive())) { LB_GL(glBindBuffer, GL_ARRAY_BUFFER, 0); LB_GL(glVertexAttribPointer, m_location, _size, GLTypeTraits<_T>::typeEnum, GL_FALSE, _stride, _data); } }
	template <class _T> void setData(Buffer<_T> const& _b, unsigned _size = 1, unsigned _stride = 0, intptr_t _offset = 0) const { if (Assert(isActive())) { _b.sharedPtr()->bind(); LB_GL(glVertexAttribPointer, m_location, _size, GLTypeTraits<_T>::typeEnum, GL_FALSE, _stride, (GLvoid*)_offset); } }

	void enable() const { LB_GL(glEnableVertexAttribArray, m_location); }
	void disable() const { LB_GL(glDisableVertexAttribArray, m_location); }

private:
	bool isActive() const;

	std::weak_ptr<ProgramFace> m_p;
	GLint m_location;
};

}
