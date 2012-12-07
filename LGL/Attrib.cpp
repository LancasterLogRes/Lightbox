#include "GL.h"
#include "ProgramFace.h"
#include "Program.h"
#include "Uniform.h"
using namespace Lightbox;

Attrib::Attrib(Program const& _p, std::string const& _name):
	m_p			(_p.sharedPtr()),
	m_location	(_p ? LB_GL_R(glGetAttribLocation, _p.sharedPtr()->id(), _name.c_str()) : -1)
{
	if (m_location > -1)
		glEnableVertexAttribArray(m_location);
}

bool Attrib::isActive() const
{
	return ProgramFace::inUse() == m_p.lock();
}
