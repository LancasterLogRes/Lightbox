#pragma once

#include <Common/Pimpl.h>
#include "ShaderFace.h"

namespace Gav
{

class VertexShaderFace: public ShaderFace
{
public:
	VertexShaderFace(): ShaderFace(glCreateShader(GL_VERTEX_SHADER)) {}
};

class VertexShader: public Pimpl<VertexShaderFace>
{
public:
	VertexShader() {}
	VertexShader(string const& _code): Pimpl<VertexShaderFace>(new VertexShaderFace) { m_p->setSource(_code); m_p->compile(); }
};

}
