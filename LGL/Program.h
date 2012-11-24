#pragma once

#include <memory>
#include <boost/utility.hpp>
#include <Common/Pimpl.h>
#include "Uniform.h"
#include "Attrib.h"
#include "ProgramFace.h"

namespace Lightbox
{

class Program: public Pimpl<ProgramFace>
{
	friend class ProgramFace;

public:
	Program() {}
	Program(Shader const& _vs, Shader const& _fs):
		Pimpl<ProgramFace>(new ProgramFace)
	{
		m_p->setVertex(_vs);
		m_p->setFragment(_fs);
		m_p->link();
	}

	// TODO: change to "variable", then distinguish on lookup.
	Uniform operator[](std::string const& _name) { return Uniform(*this, _name); }

	Attrib attrib(std::string const& _name) const { return Attrib(*this, _name); }
	Uniform uniform(std::string const& _name) const { return Uniform(*this, _name); }

	void use() const { if (m_p) m_p->use(m_p); }
	void drop() const { if (m_p) m_p->drop(); }

	static Program inUse() { return Program(ProgramFace::inUse()); }

private:
	Program(std::shared_ptr<ProgramFace> _p): Pimpl<ProgramFace>(_p) {}
};

class ProgramUser: public boost::noncopyable
{
public:
	ProgramUser(Program const& _p): m_p(_p) { m_p.use(); }
	~ProgramUser() { m_p.drop(); }
	Program m_p;
};

}
