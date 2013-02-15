#include <Common/Global.h>
#include "GUIApp.h"
#include "Global.h"
#include "RenderToTextureSlate.h"
using namespace std;
using namespace Lightbox;

RenderToTextureSlate::RenderToTextureSlate(Texture2D const& _tex):
	Slate(iRect((iCoord)_tex.size()), iRect((iCoord)_tex.size())),
	m_tex(_tex),
	m_fb(Framebuffer::Create),
	m_fbu(m_fb)
{
	m_fbu.attachColor(m_tex);
	m_fbu.checkComplete();
	m_tex.viewport();
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);

	GUIApp::joint().u_displaySize = (fVector2)(fSize)m_tex.size();
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

RenderToTextureSlate::~RenderToTextureSlate()
{
	GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().displaySizePixels;
}

