#include <Common/Global.h>
#include <App/Display.h>
#include "Global.h"
#include "Shaders.h"
#include "Joint.h"
using namespace std;
using namespace Lightbox;

void Joint::init(Display& _d)
{
	display = &_d;
	displaySize = _d.size();

	unitQuad = Buffer<float>({ 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f });

	program = Program(Shader::vertex(LB_R(View_vert)), Shader::fragment(LB_R(View_frag)));

	program.tie(uniforms);
	uniforms["displaySize"] = (vec2)(fSize)_d.size();
	offsetScale = uniforms["offsetScale"];
	color = uniforms["color"];

	geometry = program.attrib("geometry");
}

