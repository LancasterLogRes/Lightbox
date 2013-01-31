#include <Common/Global.h>
#include <App/Display.h>
#include "Global.h"
#include "Shaders.h"
#include "Joint.h"
using namespace std;
using namespace Lightbox;

Joint::Joint(): display(nullptr)
{
	displaySizePixels = uSize(1, 1);
	displaySizeMM = fSize(1, 1);
}

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

	shaded = LB_PROGRAM(Shaders_glsl, view);
	flat = LB_PROGRAM(Shaders_glsl, flat);
	texture = LB_PROGRAM(Shaders_glsl, texture);
	general = LB_PROGRAM(Shaders_glsl, general);
	hblur6 = LB_PROGRAM_ASYM(Shaders_glsl, hblur6, hblur6);
	vblur6 = LB_PROGRAM_ASYM(Shaders_glsl, vblur6, vblur6);
	pass = LB_PROGRAM(Shaders_glsl, pass);

	shaded.tie(uniforms);
	flat.tie(uniforms);
	texture.tie(uniforms);
	general.tie(uniforms);

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

void Joint::fini()
{
	*this = Joint();
}
