#pragma once
#include <LGL.h>
#include <Common/Color.h>
#include "Font.h"

namespace Lightbox
{

class Display;

struct Joint
{
	Joint();

	void init(Display& _d);
	void fini();

	Color glow(Color _c) { return Color(_c.hue(), _c.sat() * .95f, _c.value() * 8.f, lerp(_c.sat(), .65f, glowAlpha)); }

	Display* display;
	uSize displaySizePixels;
	fSize displaySizeMM;
	UniformPage uniforms;
	PagedUniform offsetScale;
	PagedUniform gradient;
	PagedUniform color;
	Buffer<float> unitQuad;
	Buffer<float> unitCircle72;
	Program flat;
	Program shaded;
	Program texture;
	Program general;
	Program hblur6;
	Program vblur6;
	Program pass;
	Attrib flatGeometry;
	Attrib shadedGeometry;
	PagedUniform u_displaySize;
	PagedUniform u_minusY;
	unsigned glowLevels;
	float glowAlpha;
};

}
