#pragma once

#include "Pimpl.h"
#include "ShaderFace.h"

namespace Gav
{

class FragmentShaderFace: public ShaderFace
{
public:
	FragmentShaderFace(): ShaderFace(glCreateShader(GL_FRAGMENT_SHADER)) {}
};

class FragmentShader: public Pimpl<FragmentShaderFace>
{
public:
	FragmentShader() {}
	FragmentShader(string const& _code): Pimpl<FragmentShaderFace>(new FragmentShaderFace) { m_p->setSource(_code); m_p->compile(); }
};

}
