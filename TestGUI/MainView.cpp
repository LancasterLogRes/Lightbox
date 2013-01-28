#include <Common/Global.h>
#include <GUI/TextLabel.h>
#include <GUI/GUIApp.h>
#include "Blur.h"
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
		m_f = Font(100, "Ubuntu");
	}

private:
	virtual void draw(Context const& _c, unsigned)
	{
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
	}

	virtual void initGraphics()
	{
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
			m_f.draw((iCoord)(uCoord)(tex.size() / 2), "u", RGBA::White);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		// Filter it.
		m_baseTex = tex;
		m_hblur = Program(Shader::vertex(LB_RES(Blur_glsl, hblur.vert)), Shader::fragment(LB_RES(Blur_glsl, blur.frag)));
		m_hblur.tie(GUIApp::joint().uniforms);
		m_vblur = Program(Shader::vertex(LB_RES(Blur_glsl, vblur.vert)), Shader::fragment(LB_RES(Blur_glsl, blur.frag)));
		m_vblur.tie(GUIApp::joint().uniforms);
		m_glowTex = filter(filter(tex, m_hblur), m_vblur);
	}

	Texture2D filter(Texture2D const& _in, Program const& _p)
	{
		GUIApp::joint().u_displaySize = (fVector2)(fSize)_in.size();

		Framebuffer fb(Framebuffer::Create);
		FramebufferUser fbu(fb);

		Texture2D ret(_in.size());
		ret.viewport();
		fbu.attachColor(ret);
		LB_GL(glClear, GL_COLOR_BUFFER_BIT);

		ProgramUser u(_p);
		float ox = 0;
		float oy = 0;
		std::array<float, 4 * 4> quad =
		{{
			// top left.
			0, 0, ox, oy,
			// top right.
			1, 0, ox + _in.size().w(), oy,
			// bottom left.
			0, 1, ox, oy + _in.size().h(),
			// bottom right.
			1, 1, ox + _in.size().w(), oy + _in.size().h()
		}};
		u.uniform("u_tex") = _in;
		u.attrib("a_texCoordPosition").setStaticData(quad.data(), 4, 0);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		u.triangleStrip(4);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		return ret;
	}

	virtual void finiGraphics()
	{
		m_glowTex = Texture2D();
	}

	Font m_f;

	Program m_hblur;
	Program m_vblur;
	Texture2D m_baseTex;
	Texture2D m_glowTex;
};

MainView::MainView()
{
	TestViewBody::spawn(this);
	setLayout(new VerticalLayout);
}

MainView::~MainView()
{
}
