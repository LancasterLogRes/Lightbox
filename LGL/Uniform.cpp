#include "ProgramFace.h"
#include "Program.h"
#include "Uniform.h"
using namespace Lightbox;

Uniform::Uniform(Program const& _p, std::string const& _name):
	m_p			(_p.sharedPtr()),
	m_location	(_p ? LIGHTBOX_GL_RET(glGetUniformLocation(_p.sharedPtr()->id(), _name.c_str())) : -1)
{
}

/*void Uniform::set(Texture const& _t)
{
	if (m_location >= 0 && _t)
	{
		if (shared_ptr<ProgramFace> pf = m_p.lock())
			set(pf->registerSampler(_t));
		else
			gAssert(false);
	}
}
*/
