#include <Common/Global.h>
#include <App/Display.h>
#include "Global.h"
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

	shaded = Program("Shaders.glsl", "view");
	flat = Program("Shaders.glsl", "flat");
	texture = Program("Shaders.glsl", "texture");
	general = Program("Shaders.glsl", "general");
	hblur6 = Program("Shaders.glsl", "hblur6.vert", "hblur6.frag");
	vblur6 = Program("Shaders.glsl", "vblur6.vert", "vblur6.frag");
	pass = Program("Shaders.glsl", "pass");

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

	glowLevels = Lightbox::log2(ceil(_d.toPixelsF(fSize(0, 2)).h()));
	glowAlpha = 1.f - (glowLevels - 1) * .125f;
}

void Joint::fini()
{
	*this = Joint();
}
