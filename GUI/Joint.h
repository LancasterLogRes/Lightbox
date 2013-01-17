#pragma once
#include <LGL.h>
#include "Font.h"

namespace Lightbox
{

class Display;

struct Joint
{
	void init(Display& _d);

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
	Attrib flatGeometry;
	Attrib shadedGeometry;
	PagedUniform u_displaySize;
	PagedUniform u_minusY;
};

}
