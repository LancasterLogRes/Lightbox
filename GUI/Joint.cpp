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
	displaySizePixels = _d.sizePixels();
	displaySizeMM = _d.sizeMM();

	unitQuad = Buffer<float>({ 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f });
	vector<float> uc(74 * 2);
	uc[0] = uc[1] = 0.f;
	for (unsigned i = 1; i < 74; ++i)
		uc[2 * i] = sin(i / 72.f * TwoPi),
		uc[2 * i + 1] = cos(i / 72.f * TwoPi);
	unitCircle72 = Buffer<float>(uc);

	shaded = Program(Shader::vertex(LB_R(View_vert)), Shader::fragment(LB_R(View_frag)));
	flat = Program(Shader::vertex(LB_R(Flat_vert)), Shader::fragment(LB_R(Flat_frag)));
	texture = Program(Shader::vertex(LB_R(Texture_vert)), Shader::fragment(LB_R(Texture_frag)));

	shaded.tie(uniforms);
	flat.tie(uniforms);
	texture.tie(uniforms);

	u_displaySize = uniforms["displaySize"];
	u_minusY = uniforms["minusY"];
	offsetScale = uniforms["offsetScale"];
	color = uniforms["color"];
	gradient = uniforms["gradient"];

	u_displaySize = (vec2)(fSize)_d.sizePixels();
	u_minusY = -1.f;

	flatGeometry = flat.attrib("geometry");
	shadedGeometry = shaded.attrib("geometry");
}

