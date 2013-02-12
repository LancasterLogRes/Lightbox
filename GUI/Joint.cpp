#include <Common/Global.h>
#include <App/Display.h>
#include "GUIApp.h"
#include "Global.h"
#include "Joint.h"
#include "View.h"
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
	{
		vector<float> uc(74 * 2);
		uc[0] = uc[1] = 0.f;
		for (unsigned i = 1; i < 74; ++i)
			uc[2 * i] = sin(i / 72.f * TwoPi),
			uc[2 * i + 1] = cos(i / 72.f * TwoPi);
		unitCircle72 = Buffer<float>(uc);
	}

	shaded = Program("Shaders.glsl", "view");
	flat = Program("Shaders.glsl", "flat");
	texture = Program("Shaders.glsl", "texture");
	general = Program("Shaders.glsl", "general");
	hblur = Program("Shaders.glsl", "hblur4");
	vblur = Program("Shaders.glsl", "vblur4");
	pass = Program("Shaders.glsl", "pass");
	hblur6 = Program("Shaders.glsl", "hblur6");
	vblur6 = Program("Shaders.glsl", "vblur6");
	hblur8 = Program("Shaders.glsl", "hblur8");
	vblur8 = Program("Shaders.glsl", "vblur8");
	colorize = Program(texture.vertexShader(), "Shaders.glsl", "colorize");

	shaded.tie(uniforms);
	flat.tie(uniforms);
	texture.tie(uniforms);
	general.tie(uniforms);
	colorize.tie(uniforms);

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
	lightEdgePixels = display->toUnalignedPixels(GUIApp::style().lightEdgeSize);
	lightBedPixels = display->toUnalignedPixels(GUIApp::style().lightBedSize);
	glowPixels = iSize(1 << (glowLevels * 2 + 1));
	cnote << "glowLevels:" << glowLevels << "glowPixels:" << glowPixels << "lightEdgePixels:" << lightEdgePixels;
	glowAlpha = 1.f - (glowLevels - 1) * .125f;

	glowThumbTex = makeGlowerFar(thumbTex());
	glowCornerTex = makeGlowerNear(cornerTex());
}

void Joint::fini()
{
	*this = Joint();
}

Texture2D Joint::thumbTex() const
{
	iSize thumbRadiusPx = display->toUnalignedPixels(GUIApp::style().thumbDiameter / 2);
	uSize totalSize = (uSize)thumbRadiusPx * 4;	// 2x diameter
	Texture2D ret(totalSize, foreign_vector<uint8_t>(), GL_RGBA, GL_RGBA);

	Framebuffer fb(Framebuffer::Create);
	FramebufferUser u(fb);
	u.attachColor(ret);
	u.checkComplete();
	ret.viewport();
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);

	u_displaySize = (fVector2)(fSize)totalSize;
	LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	Context().disc(iEllipse(iCoord(totalSize / 2), thumbRadiusPx), Color(1.f / (glowLevels * 2 + 1)));

	u_displaySize = (fVector2)(fSize)displaySizePixels;
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return ret;
}

Texture2D Joint::makeGlowerFar(Texture2D _baseTex) const
{
	// Filter it.
	vector<Texture2D> levels(glowLevels * 2 + 1);
	for (unsigned i = 0; i < levels.size(); ++i)
		levels[i] = (i ? levels[i - 1] : _baseTex).filter(pass, Texture2D(_baseTex.size() / (1 << i)));
	for (unsigned i = 0; i < levels.size(); ++i)
		levels[i] = levels[i].filter(vblur8).filter(hblur8);

	Framebuffer fb(Framebuffer::Create);
	FramebufferUser fbu(fb);
	Texture2D ret(_baseTex.size());
	ret.viewport();
	fbu.attachColor(ret);
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
	ProgramUser u(pass);
	for (auto const& l: levels)
		u.filterMerge(l);
	u.filterMerge(_baseTex);

	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return ret;
}

Texture2D Joint::makeGlowerNear(Texture2D _baseTex) const
{
	// Filter it.
	LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	vector<Texture2D> levels(glowLevels);
	for (unsigned i = 0; i < levels.size(); ++i)
		levels[i] = (i ? levels[i - 1] : _baseTex).filter(pass, Texture2D(_baseTex.size() / (glowLevels << i)));
	for (unsigned i = 0; i < levels.size(); ++i)
		levels[i] = levels[i].filter(vblur).filter(hblur);

	Framebuffer fb(Framebuffer::Create);
	FramebufferUser fbu(fb);
	Texture2D ret(_baseTex.size());
	ret.viewport();
	fbu.attachColor(ret);
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
	ProgramUser u(pass);
	for (auto const& l: levels)
		u.filterMerge(l);
	u.filterMerge(_baseTex);

	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	return ret;
}

Texture2D Joint::cornerTex() const
{
	uSize totalSizePx = uSize(lightEdgePixels + glowPixels * 3);
	Texture2D ret(totalSizePx);
	RenderToTextureContext c(ret);
	c.rectInline(iRect((iCoord)glowPixels, (iSize)totalSizePx * 2), iMargin(lightEdgePixels), Color(1.f / (glowLevels * 2 + 1)));
	return ret;
}
