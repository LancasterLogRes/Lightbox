#pragma once

#include <LGL/Global.h>
#include <LGL/Program.h>
#include <LGL/Texture2D.h>
#include <LGL/Framebuffer.h>
#include "Slate.h"

namespace Lightbox
{

class RenderToTextureSlate: public Slate
{
public:
	RenderToTextureSlate(Texture2D const& _tex);
	~RenderToTextureSlate();

private:
	Texture2D m_tex;
	Framebuffer m_fb;
	FramebufferUser m_fbu;
};

}


