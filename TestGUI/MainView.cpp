#include <Common/Common.h>
#include <GUI/TextLabel.h>
#include <GUI/ToggleButton.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "MainView.h"
using namespace std;
using namespace Lightbox;

class TestViewBody;
typedef boost::intrusive_ptr<TestViewBody> TestView;

class RenderToTextureContext: public Context
{
public:
	RenderToTextureContext(Texture2D const& _tex): Context(iRect((iCoord)_tex.size()), iRect((iCoord)_tex.size())), m_tex(_tex), m_fb(Framebuffer::Create), m_fbu(m_fb)
	{
		m_fbu.attachColor(m_tex);
		m_fbu.checkComplete();
		m_tex.viewport();
		LB_GL(glClear, GL_COLOR_BUFFER_BIT);

		GUIApp::joint().u_displaySize = (fVector2)(fSize)m_tex.size();
		LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	~RenderToTextureContext()
	{
		GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().displaySizePixels;
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

private:
	Texture2D m_tex;
	Framebuffer m_fb;
	FramebufferUser m_fbu;
};

static const fSize c_lightSize = fSize(2, 2);

class TestViewBody: public ViewCreator<ViewBody, TestViewBody>
{
	typedef ViewCreator<ViewBody, TestViewBody> Super;

public:
	TestViewBody()
	{
//		m_f = Font(100, "ubuntu");
		BasicButton b = BasicButtonBody::spawn(this, "Hello", Red);
		b->setGeometry(fRect(100, 60, 600, 360));
		b->setOnTapped([=](ViewBody* _v){ _v->update(); });
	}

private:
	Texture2D m_cornerTex;
	Texture2D m_baseTex;
	mutable iSize m_lightSizePx;
	mutable iSize m_convPx;

	Texture2D cornerTex() const
	{
		m_lightSizePx = GUIApp::joint().display->toUnalignedPixels(c_lightSize);
		m_convPx = iSize(1 << (GUIApp::joint().glowLevels * 2 + 1));

		uSize totalSizePx = uSize(m_lightSizePx + m_convPx * 3);
		Texture2D ret(totalSizePx);
		RenderToTextureContext c(ret);
		c.rectInline(iRect((iCoord)m_convPx, (iSize)totalSizePx * 2), iMargin(m_lightSizePx), Color(1.f / (GUIApp::joint().glowLevels * 2 + 1)));
		return ret;
	}

	void glowRect(iRect _upperLeftEdgeOfLight, Color _col, float _overglow = 1.f) const
	{
		Context _c;
		ProgramUser u(GUIApp::joint().colorize);
		u.uniform("u_color") = (fVector4)_col.toRGBA();
		u.uniform("u_amplitude") = 5.f;
		u.uniform("u_overglow") = _overglow;
		iSize cornerSize = m_convPx * 2 + m_lightSizePx;
		_c.blit(iRect(cornerSize), m_cornerTex, iRect(_upperLeftEdgeOfLight.topLeft() - m_convPx, iSize(0, 0)));
		_c.blit(iRect(cornerSize).flippedHorizontal(), m_cornerTex, iRect(_upperLeftEdgeOfLight.topRight() - m_convPx, iSize(0, 0)));
		_c.blit(iRect(cornerSize).flippedVertical(), m_cornerTex, iRect(_upperLeftEdgeOfLight.bottomLeft() - m_convPx, iSize(0, 0)));
		_c.blit(iRect(cornerSize).flippedHorizontal().flippedVertical(), m_cornerTex, iRect(_upperLeftEdgeOfLight.bottomRight() - m_convPx, iSize(0, 0)));
		iSize extra = _upperLeftEdgeOfLight.size() - m_lightSizePx - m_convPx * 2;
		_c.blit(iRect(m_convPx.w() * 2 + m_lightSizePx.w(), 0, 1, m_convPx.h() * 2 + m_lightSizePx.h()), m_cornerTex, iRect(_upperLeftEdgeOfLight.x() + m_convPx.w() + m_lightSizePx.w(), _upperLeftEdgeOfLight.y() - m_convPx.w(), extra.w(), 0));
		_c.blit(iRect(m_convPx.w() * 2 + m_lightSizePx.w(), 0, 1, m_convPx.h() * 2 + m_lightSizePx.h()).flippedVertical(), m_cornerTex, iRect(_upperLeftEdgeOfLight.x() + m_convPx.w() + m_lightSizePx.w(), _upperLeftEdgeOfLight.bottom() - m_convPx.h(), extra.w(), 0));
		_c.blit(iRect(0, m_convPx.h() * 2 + m_lightSizePx.h(), m_convPx.w() * 2 + m_lightSizePx.w(), 1), m_cornerTex, iRect(_upperLeftEdgeOfLight.x() - m_convPx.h(), _upperLeftEdgeOfLight.y() + m_convPx.h() + m_lightSizePx.h(), 0, extra.h()));
		_c.blit(iRect(0, m_convPx.h() * 2 + m_lightSizePx.h(), m_convPx.w() * 2 + m_lightSizePx.w(), 1).flippedHorizontal(), m_cornerTex, iRect(_upperLeftEdgeOfLight.right() - m_convPx.h(), _upperLeftEdgeOfLight.y() + m_convPx.h() + m_lightSizePx.h(), 0, extra.h()));
	}

	void glowRectOutline(iRect _inner, Color _col, float _overglow = 1.f) const
	{
		glowRect(_inner.outset(iMargin(-m_lightSizePx.w(), -m_lightSizePx.h(), 0, 0)), _col, _overglow);
	}

	void glowRectInline(iRect _outer, Color _col, float _overglow = 1.f) const
	{
		glowRect(_outer.inset(iMargin(0, 0, m_lightSizePx.w(), m_lightSizePx.h())), _col, _overglow);
	}

	virtual void initGraphics()
	{
		m_baseTex = cornerTex();
		m_cornerTex = GUIApp::joint().makeGlower4(m_baseTex);
//		setLayers({{ Layer(iMargin(), true) }});
	}

	virtual void draw(Context const& _c, unsigned)
	{
		Color c(mod1(toSeconds(GUIApp::runningTime()) / 10), 1.f, 1.f);
		{
			fCoord p(GUIApp::style().thumbDiameter / 2.f);
			p += fSize(200, 0);
			_c.blitThumb(p, Color(mod1(toSeconds(GUIApp::runningTime()) / 10), 1.f, 1.f));
			_c.blitThumb(p + GUIApp::style().thumbDiameter * fSize(3, 0), c, 1.f);
		}
		{
			iCoord p(0, 0);
			_c.blit(m_baseTex, p);
			_c.blit(m_cornerTex, p + (iSize)m_baseTex.size() * iSize(1, 0));
			ProgramUser u(GUIApp::joint().colorize);
			u.uniform("u_color") = (fVector4)c.toRGBA();
			u.uniform("u_amplitude") = 5.f;
			u.uniform("u_overglow") = 0.f;
			_c.blit(m_cornerTex, p + (iSize)m_baseTex.size() * iSize(2, 0));
			u.uniform("u_overglow") = 1.f;
			_c.blit(m_cornerTex, iRect(p + (iSize)m_baseTex.size() * iSize(3, 0), iSize(0, 0)));
		}

		glowRectOutline(iRect(0, m_baseTex.size().h(), m_baseTex.size().w(), m_baseTex.size().h()), c, 1.f);
	}

	virtual void finiGraphics()
	{
	}

	Font m_f;
};

MainView::MainView()
{
	TestViewBody::spawn(this);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
