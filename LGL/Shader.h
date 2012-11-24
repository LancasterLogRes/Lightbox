#pragma once

#include <memory>
#include <boost/utility.hpp>
#include <Common/Pimpl.h>
#include <Common/Global.h>
#include "Uniform.h"
#include "ShaderFace.h"

namespace Lightbox
{

class Shader: public Pimpl<ShaderFace>
{
public:
	Shader() {}

	static Shader vertex(foreign_vector<uint8_t const> const& _code) { return Shader(new ShaderFace(_code, false)); }
	static Shader fragment(foreign_vector<uint8_t const> const& _code) { return Shader(new ShaderFace(_code, true)); }

private:
	Shader(ShaderFace* _p): Pimpl<ShaderFace>(std::shared_ptr<ShaderFace>(_p)) {}
};

}
