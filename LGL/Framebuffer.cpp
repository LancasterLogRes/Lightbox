#include <Common/Global.h>
#include <LGL/GLMap.h>
#include "Global.h"
#include "Framebuffer.h"
using namespace std;
using namespace Lightbox;

int FramebufferFace::s_current = 0;

FramebufferFace::FramebufferFace()
{
	LB_GL(glGenFramebuffers, 1, &m_id);
}

FramebufferFace::~FramebufferFace()
{
	LB_GL(glDeleteFramebuffers, 1, &m_id);
}

bool FramebufferUser::checkComplete() const
{
	GLenum r = LB_GL_R(glCheckFramebufferStatus, GL_FRAMEBUFFER);
	if (r != GL_FRAMEBUFFER_COMPLETE)
		cwarn << "Incomplete framebuffer: " << g_symbolStrings.at(r);
	return r == GL_FRAMEBUFFER_COMPLETE;
}
