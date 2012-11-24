#pragma once

#include <Common/Pimpl.h>
#include <Common/Global.h>
#include "BufferFace.h"

namespace Lightbox
{

template <class _T>
class Buffer: public Pimpl<BufferFace<_T> >
{
public:
	Buffer() {}
	explicit Buffer(std::vector<_T> const& _data, GLenum _usage = GL_STATIC_DRAW): Pimpl<BufferFace<_T> >(new BufferFace<_T>) { m_p->data(_data.size(), _data.data(), _usage); }
	explicit Buffer(foreign_vector<_T const> const& _data, GLenum _usage = GL_STATIC_DRAW): Pimpl<BufferFace<_T> >(new BufferFace<_T>) { m_p->data(_data.size(), _data.data(), _usage); }

	void set(foreign_vector<_T const> const& _data, unsigned _first = 0) { if (m_p) m_p->subData(_first, _data.size(), _data.data()); }
	void set(foreign_vector<_T const> const& _data, unsigned _count, unsigned _first = 0) { if (m_p) m_p->subData(_first, _count, _data.data()); }
	void set(_T const* _data, unsigned _count, unsigned _first = 0) { if (m_p) m_p->subData(_first, _count, _data); }

	using Pimpl<BufferFace<_T> >::m_p;
};

}
