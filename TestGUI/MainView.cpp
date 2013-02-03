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
		m_f = Font(100, "ubuntu");

		ToggleButtonBody::spawn(this, "Hello", Red, Font(16, "AbyssinicaSIL"))->setGeometry(fRect(400, 20, 150, 50));

	}

private:
	virtual void preDraw(unsigned)
	{
		return;
		LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		m_glowTex = m_baseTex.filter(m_vblur).filter(m_hblur);
		vector<Texture2D> levels(3);
		for (unsigned i = 0; i < levels.size(); ++i)
			levels[i] = (i ? levels[i - 1] : m_baseTex).filter(m_pass, Texture2D(m_baseTex.size() / (1 << i)));
		for (unsigned i = 0; i < levels.size(); ++i)
			levels[i] = levels[i].filter(m_vblur).filter(m_hblur).filter(m_hblur);
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Framebuffer fb(Framebuffer::Create);
		FramebufferUser fbu(fb);
		m_sglowTex = Texture2D(m_baseTex.size());
		m_sglowTex.viewport();
		fbu.attachColor(m_sglowTex);
		LB_GL(glClear, GL_COLOR_BUFFER_BIT);
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE);
		ProgramUser u(m_pass);
		for (auto const& l: levels)
			u.filterMerge(l);
		u.filterMerge(m_baseTex);
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	virtual void draw(Context const& _c, unsigned)
	{
		_c.disc(iEllipse(128, 58, 15, 15), Color(0, .95, 2, .35f));
		_c.rectOutline(iRect(50, 50, 150, 175), iMargin(iSize(8, 8)), Color(0, .95f, 2.f, .35f));
/*
		fRect r = m_f.measure("Hello world", true).translated(fSize(100, 150));
		fRect r2 = m_f.measure("Hello world", false).translated(fSize(100, 250));
		fRect r3 = m_f.getBaked()->measureMm("Hello world").translated(fSize(100, 350));
		_c.rect(r, Color(0, 1, 1, .5f));
		_c.rect(r2, Color(.33, 1, 1, .5f));
		_c.rect(r3, Color(.66, 1, 1, .5f));
		m_f.draw(_c.toDevice(r.lerp(.5, .5)), "Hello world", RGBA::Red, AtCenter);
		m_f.draw(_c.toDevice(r2.lerp(.5, .5)), "Hello world", RGBA::Green, AtCenter);
		m_f.draw(_c.toDevice(r3.lerp(.5, .5)), "Hello world", RGBA::Blue, AtCenter);

		_c.blit(m_baseTex);
		_c.blit(m_glowTex, fCoord(0, 256));
		_c.blit(m_sglowTex, fCoord(560, 0));*/
	}

	virtual void initGraphics()
	{
		setLayers(Layers(1, Layer(iMargin(), true)));

		uSize s(256, 256);
		Texture2D tex(s);

		// Make initial texture.
		{
			Framebuffer fb(Framebuffer::Create);
			FramebufferUser u(fb);
			u.attachColor(tex);
			tex.viewport();
			LB_GL(glClear, GL_COLOR_BUFFER_BIT);
			GUIApp::joint().u_displaySize = (fVector2)(fSize)s;
			glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//			m_f.draw((iCoord)(uCoord)(tex.size() / 2), "x", RGBA::Blue);
			Context c;
			c.disc(iEllipse(128, 58, 15, 15), Color(0, .95, 2, .4f));
			c.rectOutline(iRect(50, 50, 150, 175), iMargin(iSize(8, 8)), Color(0, .95, 2, .4f));
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().displaySizePixels;
		}

		// Filter it.
		m_baseTex = tex;
		m_hblur = Program("Blur.glsl", "hblur6.vert", "blur6.frag");
		m_vblur = Program("Blur.glsl", "vblur6.vert", "blur6.frag");
		m_pass = Program("Blur.glsl", "pass");

		GUIApp::joint().display->setAnimating();
	}

	virtual void finiGraphics()
	{
		GUIApp::joint().display->releaseAnimating();
		m_hblur = Program();
		m_vblur = Program();
		m_baseTex = Texture2D();
		m_glowTex = Texture2D();
		m_sglowTex = Texture2D();
	}

	Font m_f;

	Program m_hblur;
	Program m_vblur;
	Program m_pass;
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
