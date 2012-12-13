#include <LGL/GL.h>
#include <Common/StreamIO.h>
#include <Common/Global.h>
#include <LGL.h>
#include <App.h>
#include "Fonts.h"
#include "Global.h"
#include "View.h"
#include "Shaders.h"
#include "Frame.h"
#include "GUIApp.h"
using namespace std;
using namespace Lightbox;

void Style::generateColors(Color _fore)
{
	fore = _fore;
	back = Color(_fore.h(), _fore.sat() / 2.f, _fore.value() / 2.f);
	high = Color(_fore.h() + .5f, _fore.sat(), _fore.value());
}

GUIApp::GUIApp()
{
	m_root = FrameBody::create();
}

GUIApp::~GUIApp()
{
}

void GUIApp::initGraphics(Display& _d)
{
	cnote << "Version" << (char const*)glGetString(GL_VERSION);
	cnote << "Vendor" << (char const*)glGetString(GL_VENDOR);
	cnote << "Renderer" << (char const*)glGetString(GL_RENDERER);
	cnote << "Extensions" << (char const*)glGetString(GL_EXTENSIONS);

	LB_GL(glDisable, GL_CULL_FACE);
	LB_GL(glDisable, GL_DEPTH_TEST);
	LB_GL(glEnable, GL_BLEND);
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_joint.init(_d);
	m_style.regular = Font(ubuntu_r_ttf, 16.f);
	m_style.bold = Font(ubuntu_b_ttf, 16.f);
	m_style.fore = Color(RGB8Space, 0x86, 0x9f, 0xc3);
	m_style.back = Color(RGBSpace, .4f, .5f, .6f, 1.f);
	m_style.high = Color(RGB8Space, 0xf3, 0xa3, 0x19);
	m_root->setGeometry(fRect(0, 0, _d.width(), _d.height()));
}

static const uSize s_pageSize(2048, 2048);

GUIApp::ImageCache::ImageCache():
	fb(Framebuffer::Create),
	tx(s_pageSize),
	nextfree(0)
{
	FramebufferUser u(fb);
	u.attachColor(tx);
}

bool GUIApp::ImageCache::fit(iRect _g, ViewBody* _v)
{
	// NOTE: should use a good algorithm really - http://cgi.csc.liv.ac.uk/~epa/surveyhtml.html
	float tolerance = 1.2f;

	// find a position to fit _g in our page.
	auto it = rows.lower_bound(_g.height());
	while (true)
	{
		if (it == rows.end() || it->first > _g.height() * tolerance)
		{
			// no row exists that's good. make our own?
			if (int(s_pageSize.height() - nextfree) < _g.height())
			{
				// no - not enough space left.
				if (tolerance != 1e8f)
				{
					// try restarting without tolerance
					tolerance = 1e8f;
					it = rows.lower_bound(_g.height());
					continue;
				}
				else
					return false;
			}
			// yes - add to bottom
			it = rows.insert(make_pair(_g.height(), make_pair(nextfree, 0)));
			nextfree += _g.height();
		}
		if (int(s_pageSize.width() - it->second.second) >= _g.width())
		{
			// yey - enough space on the row.
			uRect tr = uRect(it->second.second, it->second.first, _g.width(), _g.height());
			vs[_v].pos = tr;
			vs[_v].index = collated.size() / (6 * 4);
			fRect ftr = fRect(tr) / fSize(s_pageSize);
			fRect gr = fRect(_g);
			array<float, 6 * 4> toCopy =
			{{
				ftr.left(), ftr.top(), gr.left(), gr.top(),
				ftr.left(), ftr.bottom(), gr.left(), gr.bottom(),
				ftr.right(), ftr.top(), gr.right(), gr.top(),
				ftr.left(), ftr.bottom(), gr.left(), gr.bottom(),
				ftr.right(), ftr.top(), gr.right(), gr.top(),
				ftr.right(), ftr.bottom(), gr.right(), gr.bottom()
			}};
			collated.resize(collated.size() + toCopy.size());
			valcpy(&*collated.end() - toCopy.size(), toCopy.data(), toCopy.size());
			it->second.second += _g.width();
			return true;
		}
		++it;
	}
}

bool GUIApp::drawGraphics()
{
	bool stillDirty = false;

	vector<ViewBody*> drawers;
	m_root->gatherDrawers(drawers);
	if (find_if(drawers.begin(), drawers.end(), [](ViewBody* v){ return v->m_visibleLayoutChanged; }) != drawers.end())
	{
		// At least one resized drawer - reset and redraw everything (in the future we might attempt a more evolutionary cache design).
		m_cache.clear();

		for (ViewBody* v: drawers)
		{
			v->m_dirty = true;
			v->m_visibleLayoutChanged = false;
			while (true)
			{
				int page = max(0, int(m_cache.size()) - 1);
				// assuming page is valid, try to find a position in m_cache[page] and put it into pos...
				if ((int)m_cache.size() > page && m_cache[page].fit((iRect)fRect(v->m_globalPosAsOfLastGatherDrawers.rounded(), v->geometry().size().rounded()), v))
					break;
				// End of cache - add a new page onto m_cache.
				m_cache.push_back(ImageCache());
			}
		}
		for (auto& c: m_cache)
		{
			c.geom = Buffer<float>(c.collated);
			c.collated.clear();
		}
	}

	vector< vector<ViewBody*> > renderToFramebuffer(m_cache.size());

	if (find_if(drawers.begin(), drawers.end(), [&](ViewBody* v){ return v->m_dirty; }) != drawers.end())
	{
		// At least one dirty drawer - set up for render-to-texture and rerender dirty parts.
		LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		LB_GL(glClearColor, 0.f, 0.f, 0.f, 0.f);
		LB_GL(glEnable, GL_SCISSOR_TEST);

		unsigned ci = 0;
		for (auto& c: m_cache)
		{
			bool willRenderToTexture = false;
			for (auto const& v: c.vs)
			{
				if (v.first->m_dirty && v.first->m_wasDirty)
					willRenderToTexture = true;
				else if (v.first->m_dirty)
				{
					renderToFramebuffer[ci] += v.first;
					v.first->m_wasDirty = stillDirty = true;
				}
			}

			sort(renderToFramebuffer[ci].begin(), renderToFramebuffer[ci].end(), [&](ViewBody* a, ViewBody* b) { return c.vs[a].index < c.vs[b].index; });

			if (willRenderToTexture)
			{
				FramebufferUser u(c.fb);
				for (auto const& v: c.vs)
					if (v.first->m_dirty)
					{
						uRect texRect = v.second.pos;
						LB_GL(glViewport, texRect.x(), texRect.y(), texRect.w(), texRect.h());
						LB_GL(glScissor, texRect.x(), texRect.y(), texRect.w(), texRect.h());
						LB_GL(glClear, GL_COLOR_BUFFER_BIT);
						GUIApp::joint().u_displaySize = (vec2)(fSize)texRect.size();
						v.first->draw(Context());
						if (!v.first->m_isEnabled)
							Context().rect(fRect(fCoord(0, 0), (fSize)texRect.size()), Color(0.f, .5f));
						v.first->m_dirty = false;
					}
			}
			ci++;
		}
		LB_GL(glDisable, GL_SCISSOR_TEST);
	}

	// Cache up to date - now to composite.
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LB_GL(glClearColor, 0, 0, 0, 0);
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);
	LB_GL(glViewport, 0, 0, GUIApp::joint().display->size().w(), GUIApp::joint().display->size().h());
	GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().display->size() * vec2(1, -1);
	{
		// geometry is guaranteed to be in composite-draw-order.
		unsigned ci = 0;
		for (auto& c: m_cache)
		{
			unsigned next = 0;
			for (ViewBody* v: renderToFramebuffer[ci])
			{
				assert(c.vs.count(v));
				CachePos const& cp = c.vs[v];
				// render all before
				if (next < cp.index)
				{
					ProgramUser u(m_joint.texture);
					u.uniform("u_tex") = c.tx;
					u.attrib("a_texCoordPosition").setData(c.geom, 4, 0, 6 * 4 * next * sizeof(float));
					u.triangles((cp.index - next) * 6);
				}

				// draw our view directly to framebuffer.
				Context c;
				c.offset = v->m_globalPosAsOfLastGatherDrawers;
				debugOut(v);
				LB_GL(glEnable, GL_SCISSOR_TEST);
				LB_GL(glScissor, round(v->m_globalPosAsOfLastGatherDrawers.x()), GUIApp::joint().display->size().h() - round(v->geometry().h()) - round(v->m_globalPosAsOfLastGatherDrawers.y()), round(v->geometry().w()), round(v->geometry().h()));
				v->draw(c);
				if (!v->m_isEnabled)
					c.rect(fRect(0, 0, round(v->geometry().w()), round(v->geometry().h())), Color(0.f, .5f));
				LB_GL(glDisable, GL_SCISSOR_TEST);
				next = cp.index + 1;
			}

			// render from next to end.
			if (next < c.vs.size())
			{
				ProgramUser u(m_joint.texture);
				u.uniform("u_tex") = c.tx;
				u.attrib("a_texCoordPosition").setData(c.geom, 4, 0, 6 * 4 * next * sizeof(float));
				u.triangles((c.vs.size() - next) * 6);
			}
		}
	}

	string info = textualTime(AppEngine::get()->lastDrawTime());
	Context().rect(fRect(fCoord(m_root->geometry().size()) - fCoord(200, 54), fSize(190, 44)), Color(1.f, .5f));
	fSize s = GUIApp::style().regular.measure(info);
	GUIApp::style().regular.draw(fCoord(m_root->geometry().size() - s / 2.f - fSize(34, 34)), info, RGBA::Black);
	info = toString(g_metrics.m_useProgramCount) + "/" + toString(g_metrics.m_drawCount);
	s = GUIApp::style().regular.measure(info);
	GUIApp::style().regular.draw(fCoord(m_root->geometry().size() - s / 2.f - fSize(33, 14)), info, RGBA::Black);
	g_metrics.reset();

	return !stillDirty;
}

bool GUIApp::motionEvent(int _id, iCoord _pos, int _direction)
{
	TouchEvent* ev = nullptr;
	if (_direction > 0)
		ev = new TouchDownEvent(_id, _pos);
	else if (_direction < 0)
		ev = new TouchUpEvent(_id, _pos);
	else if (m_pointerLock[_id])
		ev = new TouchMoveEvent(_id, _pos);

	if (!ev)
		return false;

	bool ret = false;
	if (m_pointerLock[_id])
	{
		auto gp = m_pointerLock[_id]->parent()->globalPos();
		ev->local -= gp;
		ret = m_pointerLock[_id]->handleEvent(ev);
		ev->local += gp;
	}
	if (!ret)
		ret = m_root->handleEvent(ev);

	delete ev;

	if (_direction < 0)
		m_pointerLock[_id] = nullptr;

	return ret;
}

GUIApp* GUIApp::get()
{
	return dynamic_cast<GUIApp*>(AppEngine::get()->app());
}

bool GUIApp::pointerLocked(int _id, View const& _v) const
{
	return m_pointerLock[_id] == _v;
}

bool GUIApp::lockPointer(int _id, View const& _v)
{
	if (m_pointerLock[_id] == nullptr)
	{
		m_pointerLock[_id] = _v;
		return true;
	}
	return false;
}

bool GUIApp::releasePointer(int _id, View const& _v)
{
	if (m_pointerLock[_id] == _v)
	{
		m_pointerLock[_id] = nullptr;
		return true;
	}
	return false;
}
