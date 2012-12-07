#pragma once

#include <set>
#include <map>
#include <list>
#include "GL.h"
#include "Texture2D.h"
#include "Shader.h"

namespace Lightbox
{

class UniformPage;
class UniformPageFace;

class ProgramFace: public boost::noncopyable
{
	friend class Uniform;	// for m_nv

public:
	ProgramFace(): m_id(LB_GL_RE(glCreateProgram)) {}
	~ProgramFace();

	void setVertex(Shader const& _v) { m_v = _v; }
	void setFragment(Shader const& _f) { m_f = _f; }

	GLuint id() const { return m_id; }
	Shader const& vertexShader() const { return m_v; }
	Shader const& fragmentShader() const { return m_f; }
	void link() const;
	void use(std::shared_ptr<ProgramFace> const& _this) const;
	void drop() const;

	int uniformLocation(std::string const& _name) const { return LB_GL_R(glGetUniformLocation, m_id, _name.c_str()); }

	int registerSampler(Texture2D const& _t);

	void tie(UniformPage const& _p);
	void untie(UniformPage const& _p);

	static std::shared_ptr<ProgramFace> const& inUse() { return s_inUse; }

private:
	mutable std::vector<Texture2D> m_reg;
	Shader m_v;
	Shader m_f;
	GLuint m_id;
	std::map<GLuint, std::function<void()> > m_nv;
	std::set<std::shared_ptr<UniformPageFace> > m_uniformPages;

	static std::shared_ptr<ProgramFace> s_inUse;
};

}
