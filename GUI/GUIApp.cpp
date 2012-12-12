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

bool GUIApp::ImageCache::fit(fRect _g, ViewBody* _v)
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
			if (s_pageSize.height() - nextfree < _g.height())
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
		if (s_pageSize.width() - it->second.second >= _g.width())
		{
			// yey - enough space on the row.
			uRect tr = uRect(it->second.second, it->second.first, _g.width(), _g.height());
			vs[_v] = tr;
			fRect ftr = fRect(tr) / fSize(s_pageSize);
			array<float, 6 * 4> toCopy =
			{{
				ftr.left(), ftr.top(), _g.left(), _g.top(),
				ftr.left(), ftr.bottom(), _g.left(), _g.bottom(),
				ftr.right(), ftr.top(), _g.right(), _g.top(),
				ftr.left(), ftr.bottom(), _g.left(), _g.bottom(),
				ftr.right(), ftr.top(), _g.right(), _g.top(),
				ftr.right(), ftr.bottom(), _g.right(), _g.bottom()
			}};
			collated.resize(collated.size() + toCopy.size());
			valcpy(&*collated.end() - toCopy.size(), toCopy.data(), toCopy.size());
			it->second.second += _g.width();
			return true;
		}
		++it;
	}
}

void GUIApp::drawGraphics()
{
	vector<ViewBody*> drawers;
	m_root->gatherDrawers(drawers);
	if (find_if(drawers.begin(), drawers.end(), [](ViewBody* v){ return v->m_dirtySize; }) != drawers.end())
	{
		// At least one resized drawer - clear cache first.
		m_cache.clear();

		for (ViewBody* v: drawers)
		{
//			int page = 0;
			while (true)
			{
				int page = max(0, int(m_cache.size()) - 1);
				if ((int)m_cache.size() > page)
				{
					// try to find a position in m_cache[page] and put it into pos...
					if (m_cache[page].fit(fRect(v->m_globalPos, v->geometry().size()), v))
						break;
				}
//				if (page == m_cache.size())
				{
					// End of cache - add a new page onto m_cache.
					m_cache.push_back(ImageCache());
				}
			}
		}
		for (auto& c: m_cache)
		{
			c.geom = Buffer<float>(c.collated);
			c.collated.clear();
		}
	}

	if (find_if(drawers.begin(), drawers.end(), [](ViewBody* v){ return v->m_dirty || v->m_dirtySize; }) != drawers.end())
	{
		// At least one dirty drawer - set up for render-to-texture and rerender dirty parts.
		LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		LB_GL(glClearColor, 0.f, 0.f, 0.f, 0.f);
		LB_GL(glEnable, GL_SCISSOR_TEST);

		for (auto& c: m_cache)
			if (find_if(c.vs.begin(), c.vs.end(), [](pair<ViewBody*, uRect> v){ return v.first->m_dirty || v.first->m_dirtySize; }) != c.vs.end())
			{
				FramebufferUser u(c.fb);
				for (auto const& v: c.vs)
					if (v.first->m_dirty || v.first->m_dirtySize)
					{
						LB_GL(glViewport, v.second.x(), v.second.y(), v.second.w(), v.second.h());
						LB_GL(glScissor, v.second.x(), v.second.y(), v.second.w(), v.second.h());
						LB_GL(glClear, GL_COLOR_BUFFER_BIT);
						GUIApp::joint().u_displaySize = (vec2)(fSize)v.second.size();
						v.first->draw(Context());
						v.first->m_dirty = false;
						v.first->m_dirtySize = false;
					}
			}

		LB_GL(glDisable, GL_SCISSOR_TEST);
	}

	// Cache up to date - now to composite.
	LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	LB_GL(glClearColor, 0, 0, 0, 0);
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);
	LB_GL(glViewport, 0, 0, GUIApp::joint().display->size().w(), GUIApp::joint().display->size().h());
	GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().display->size() * vec2(1, -1);
	ProgramUser u(m_joint.texture);
	for (auto& c: m_cache)
	{
		u.uniform("u_tex") = c.tx;
		u.attrib("a_texCoordPosition").setData(c.geom, 4);
		u.triangles(c.vs.size() * 6);

		// TODO: fold in shading (graying?) of disabled stuff while blitting.
//		if (!m_isEnabled)
//			_c.rect(m_geometry, Color(0, 0.5f));
	}

	string info = textualTime(AppEngine::get()->lastDrawTime());
	Context().rect(fRect(fCoord(m_root->geometry().size()) - fCoord(200, 54), fSize(190, 44)), Color(1.f, .5f));
	fSize s = GUIApp::style().regular.measure(info);
	GUIApp::style().regular.draw(fCoord(m_root->geometry().size() - s / 2.f - fSize(34, 34)), info, RGBA::Black);
	info = toString(g_metrics.m_useProgramCount) + "/" + toString(g_metrics.m_drawCount);
	s = GUIApp::style().regular.measure(info);
	GUIApp::style().regular.draw(fCoord(m_root->geometry().size() - s / 2.f - fSize(33, 14)), info, RGBA::Black);
	g_metrics.reset();
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
