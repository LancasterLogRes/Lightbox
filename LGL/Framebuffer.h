#pragma once

#include <Common/Pimpl.h>
#include <Common/Global.h>
#include "Texture2D.h"
#include "Global.h"

namespace Lightbox
{

class FramebufferFace
{
public:
	FramebufferFace();
	~FramebufferFace();

	void bind() { assert(s_current == 0); s_current = m_id; LB_GL(glBindFramebuffer, GL_FRAMEBUFFER, m_id); }
	void unbind() { assert(s_current != 0); s_current = 0; LB_GL(glBindFramebuffer, GL_FRAMEBUFFER, 0); }

private:
	GLuint m_id;
	static int s_current;
};

class Framebuffer: public Pimpl<FramebufferFace>
{
public:
	enum CreateType { Create };
	Framebuffer() {}
	Framebuffer(CreateType): Pimpl(new FramebufferFace) {}

	void bind() { m_p->bind(); }
	void unbind() { m_p->unbind(); }
};

class FramebufferUser: public boost::noncopyable
{
public:
	FramebufferUser(Framebuffer const& _p): m_p(_p) { m_p.bind(); }
	~FramebufferUser() { m_p.unbind(); }

	void attachColor(Texture2D const& _t, unsigned _index = 0, unsigned _level = 0) { _t.sharedPtr()->framebufferColor(_index, _level); }

	bool checkComplete() const;

private:
	Framebuffer m_p;
};

}


