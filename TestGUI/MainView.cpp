#include <Common/Global.h>
#include <GUI/TextLabel.h>
#include <GUI/ToggleButton.h>
#include <GUI/GUIApp.h>
#include "Global.h"
#include "MainView.h"
using namespace std;
using namespace Lightbox;

class TestViewBody;
typedef boost::intrusive_ptr<TestViewBody> TestView;

class TestViewBody: public ViewCreator<ViewBody, TestViewBody>
{
	typedef ViewCreator<ViewBody, TestViewBody> Super;

public:
	TestViewBody()
	{
//		m_f = Font(100, "ubuntu");

//		ToggleButtonBody::spawn(this, "Hello", Red)->setGeometry(fRect(400, 20, 150, 50));
	}

private:
	virtual void initGraphics()
	{
		GUIApp::joint().display->setAnimating();

		iSize thumbPx = GUIApp::joint().display->toUnalignedPixels(GUIApp::style().thumbSize / 2);
		uSize totalSize = (uSize)thumbPx * 4;

		int levelCount = 1;

		Texture2D tex(totalSize, foreign_vector<uint8_t>(), GL_LUMINANCE, GL_LUMINANCE);

		// Make initial texture.
		{
			Framebuffer fb(Framebuffer::Create);
			FramebufferUser u(fb);
			u.attachColor(tex);
			tex.viewport();
			LB_GL(glClear, GL_COLOR_BUFFER_BIT);

			GUIApp::joint().u_displaySize = (fVector2)(fSize)totalSize;
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			Context().disc(iEllipse(iCoord(totalSize / 2), thumbPx), Color(pow(.5f, levelCount / 2.f)));

			GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().displaySizePixels;
		}

		// Filter it.
		m_baseTex = tex;
		m_hblur = Program("Blur.glsl", "hblur6.vert", "blur6.frag");
		m_vblur = Program("Blur.glsl", "vblur6.vert", "blur6.frag");
		m_colorize = Program(GUIApp::joint().texture.vertexShader(), "Blur.glsl", "colorize");
		m_colorize.tie(GUIApp::joint().uniforms);

		m_glowTex = m_baseTex.filter(m_vblur).filter(m_hblur);
		vector<Texture2D> levels(levelCount);
		for (unsigned i = 0; i < levels.size(); ++i)
			levels[i] = (i ? levels[i - 1] : m_baseTex).filter(GUIApp::joint().pass, Texture2D(m_baseTex.size() / (1 << i)));
		for (unsigned i = 0; i < levels.size(); ++i)
			levels[i] = levels[i].filter(m_vblur).filter(m_hblur);

		Framebuffer fb(Framebuffer::Create);
		FramebufferUser fbu(fb);
		m_sglowTex = Texture2D(m_baseTex.size());
		m_sglowTex.viewport();
		fbu.attachColor(m_sglowTex);
		LB_GL(glClear, GL_COLOR_BUFFER_BIT);
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
		ProgramUser u(GUIApp::joint().pass);
		for (auto const& l: levels)
			u.filterMerge(l);
		u.filterMerge(m_baseTex);
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	virtual void draw(Context const& _c, unsigned)
	{
		ProgramUser u(m_colorize);
		u.uniform("u_color") = RGBA(1, 0.5, 0);
		_c.blit(m_sglowTex, fCoord(0, 0));
	}

	virtual void finiGraphics()
	{
		GUIApp::joint().display->releaseAnimating();
		m_hblur = Program();
		m_vblur = Program();
		m_colorize = Program();
		m_baseTex = Texture2D();
		m_glowTex = Texture2D();
		m_sglowTex = Texture2D();
	}

	Font m_f;

	Program m_hblur;
	Program m_vblur;
	Program m_colorize;
	Texture2D m_baseTex;
	Texture2D m_glowTex;
	Texture2D m_sglowTex;
};

MainView::MainView()
{
	TestViewBody::spawn(this);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
