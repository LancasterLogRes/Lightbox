#include "Program.h"
using namespace Lightbox;

void ProgramUser::filterMerge(Texture2D const& _in)
{
	std::array<float, 4 * 2> quad = {{ 0, 0, 1, 0, 0, 1, 1, 1 }};
	uniform("u_tex") = _in;
	attrib("a_position").setStaticData(quad.data(), 2, 0);
	uniform("u_texturePitch") = vec2(1.f / _in.size().w(), 1.f / _in.size().h());
	triangleStrip(4);
}
