#pragma once

#include <vector>
#include <Common/Global.h>
#include "GL.h"
#include "Global.h"

namespace lb
{

class BufferFaceGeneral: public boost::noncopyable
{
public:
	BufferFaceGeneral() { LB_GL(glGenBuffers, 1, &m_id); }
	~BufferFaceGeneral() { LB_GL(glDeleteBuffers, 1, &m_id); }

	GLuint id() const { return m_id; }
	void bind() const { LB_GL(glBindBuffer, GL_ARRAY_BUFFER, m_id); }

	void data(size_t _size, void const* _data, GLenum _usage) { bind(); LB_GL(glBufferData, GL_ARRAY_BUFFER, _size, _data, _usage); }
	void subData(size_t _offset, size_t _size, void const* _data) { bind(); LB_GL(glBufferSubData, GL_ARRAY_BUFFER, _offset, _size, _data); }

	template <class _T> void typedData(unsigned _count, _T const* _data, GLenum _usage) { data(_count * sizeof(_T), _data, _usage); }
	template <class _T> void typedSubData(unsigned _first, unsigned _count, _T const* _data) { subData(_first * sizeof(_T), _count * sizeof(_T), _data); }

protected:
	GLuint m_id;
};

template <class _T>
class BufferFace: public BufferFaceGeneral
{
public:
	void data(unsigned _count, _T const* _data, GLenum _usage) { BufferFaceGeneral::typedData<_T>(_count, _data, _usage); }
	void subData(unsigned _first, unsigned _count, _T const* _data) { BufferFaceGeneral::typedSubData<_T>(_first, _count, _data); }
};

}
