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
	PagedUniform color;
	Buffer<float> unitQuad;
	Program program;
	Attrib geometry;
	Font defaultFont;
};

}
