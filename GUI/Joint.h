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
	uSize displaySize;
	UniformPage uniforms;
	PagedUniform offsetScale;
	PagedUniform gradient;
	PagedUniform color;
	Buffer<float> unitQuad;
	Buffer<float> unitCircle72;
	Program flat;
	Program shaded;
	Attrib flatGeometry;
	Attrib shadedGeometry;
};

}
