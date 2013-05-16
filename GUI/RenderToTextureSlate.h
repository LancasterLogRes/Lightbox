#pragma once

#include <LGL/Global.h>
#include <LGL/Program.h>
#include <LGL/Texture2D.h>
#include <LGL/Framebuffer.h>
#include <Numeric/Rect.h>
#include <Numeric/Margin.h>
#include "Slate.h"

namespace lb
{

class RenderToTextureSlate: public Slate
{
public:
	RenderToTextureSlate(Texture2D const& _tex);
	RenderToTextureSlate(Framebuffer const& _fb, iRect _targetCanvas, iMargin _overdraw);
	~RenderToTextureSlate();

private:
	Texture2D m_tex;
	Framebuffer m_fb;
	FramebufferUser m_fbu;
};

}


