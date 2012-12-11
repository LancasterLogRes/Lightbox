#include <Common/Global.h>
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
