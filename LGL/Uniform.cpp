#include "ProgramFace.h"
#include "Program.h"
#include "Uniform.h"
using namespace std;
using namespace Lightbox;

Uniform::Uniform(Program const& _p, std::string const& _name):
	m_p			(_p.sharedPtr()),
	m_location	(_p ? LIGHTBOX_GL_RET(glGetUniformLocation(_p.sharedPtr()->id(), _name.c_str())) : -1)
{
}

void Uniform::set(Texture2D const& _t) const
{
	if (m_location >= 0 && _t && Assert(isActive()))
	{
		if (shared_ptr<ProgramFace> pf = m_p.lock())
			set(pf->registerSampler(_t));
		else
			assert(false);
	}
}

bool Uniform::isActive() const
{
	return ProgramFace::s_inUse == m_p.lock();
}

void Uniform::setNVAux(std::function<void()> const& _a) const
{
	if (std::shared_ptr<ProgramFace> p = m_p.lock())
		p->m_nv.insert(make_pair(m_location, _a));
}
