#include "Program.h"
#include "Shader.h"
using namespace Lightbox;

Program::Program(std::string const& _file):
	Program(Shader::vertex(Resources::find(_file, "vert")), Shader::fragment(Resources::find(_file, "frag")))
{}

Program::Program(std::string const& _file, std::string const& _base):
	Program(Shader::vertex(Resources::find(_file, _base + ".vert")), Shader::fragment(Resources::find(_file, _base + ".frag")))
{}

Program::Program(std::string const& _file, std::string const& _vert, std::string const& _frag):
	Program(Shader::vertex(Resources::find(_file, _vert)), Shader::fragment(Resources::find(_file, _frag)))
{}

void ProgramUser::filterMerge(Texture2D const& _in)
{
	std::array<float, 4 * 2> quad = {{ 0, 0, 1, 0, 0, 1, 1, 1 }};
	uniform("u_tex") = _in;
	attrib("a_position").setStaticData(quad.data(), 2, 0);
	uniform("u_texturePitch") = vec2(1.f / _in.size().w(), 1.f / _in.size().h());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	triangleStrip(4);
}

