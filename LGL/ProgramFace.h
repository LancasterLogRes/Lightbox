#pragma once

#include <ios>
#include <list>
#include <fstream>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <GLES2/gl2.h>
#include "Shader.h"

namespace Lightbox
{

class Texture;

class ProgramFace
{
public:
	ProgramFace(): m_id(glCreateProgram()) {}
	~ProgramFace();

	void setVertex(Shader const& _v) { m_v = _v; }
	void setFragment(Shader const& _f) { m_f = _f; }

	GLuint id() const { return m_id; }
	Shader const& vertexShader() const { return m_v; }
	Shader const& fragmentShader() const { return m_f; }
	void link() const;
	void use(std::shared_ptr<ProgramFace> const& _this) const;
	void drop() const;

//	int registerSampler(Texture const& _t);

	static std::shared_ptr<ProgramFace> const& inUse() { return s_inUse; }

private:
//	mutable std::vector<Texture> m_reg;
	Shader m_v;
	Shader m_f;
	GLuint m_id;

	static std::shared_ptr<ProgramFace> s_inUse;
};

}
