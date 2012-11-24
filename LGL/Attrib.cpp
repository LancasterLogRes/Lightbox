#include <GLES2/gl2.h>
#include "ProgramFace.h"
#include "Program.h"
#include "Uniform.h"
using namespace Lightbox;

Attrib::Attrib(Program const& _p, std::string const& _name):
	m_p			(_p.sharedPtr()),
	m_location	(_p ? LIGHTBOX_GL_RET(glGetAttribLocation(_p.sharedPtr()->id(), _name.c_str())) : -1)
{
	if (m_location > -1)
		glEnableVertexAttribArray(m_location);
}
