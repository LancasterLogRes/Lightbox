#include <LGL/GL.h>
#include <Common/StreamIO.h>
#include <Common/Global.h>
#include <LGL.h>
#include <App.h>
#include "Global.h"
#include "View.h"
#include "Frame.h"
#include "RenderToTextureSlate.h"
#include "GUIApp.h"
using namespace std;
using namespace lb;

void Style::generateColors(Color _fore)
{
	fore = _fore;
	back = Color(_fore.h(), _fore.sat() / 2.f, _fore.value() / 2.f);
	high = Color(_fore.h() + .5f, _fore.sat(), _fore.value());
}

GUIApp::GUIApp():
	m_startTime(wallTime()),
	m_showCachePage(-1)
{
	m_root = FrameBody::create();
	m_root->m_isRoot = true;

	m_style.fore = Color(.5f);
	m_style.back = Color(0.f);
	m_style.high = Color(1.f);
	m_style.big = Font(20, FontDefinition("ubuntu", false));
	m_style.bigBold = Font(20, FontDefinition("ubuntu", true));
	m_style.regular = Font(17, FontDefinition("ubuntu", false));
	m_style.bold = Font(17, FontDefinition("ubuntu", true));
	m_style.small = Font(14, FontDefinition("ubuntu", false));
	m_style.smallBold = Font(14, FontDefinition("ubuntu", true));
	m_style.xsmall = Font(11, FontDefinition("ubuntu", false));
	m_style.xsmallBold = Font(11, FontDefinition("ubuntu", true));
	m_style.thumbDiameter = fSize(40, 40);
	m_style.lightEdgeSize = fSize(3, 3);
	m_style.lightBedSize = fSize(5, 5);
	m_style.outlineColor = Black;
}

GUIApp::~GUIApp()
{
}

void GUIApp::go()
{
	Super::go();
	m_startTime = wallTime();
}

void GUIApp::initGraphics(Display& _d)
{
	cnote << "Initializing GUI graphics";
	cnote << "Version" << (char const*)glGetString(GL_VERSION);
	cnote << "Vendor" << (char const*)glGetString(GL_VENDOR);
	cnote << "Renderer" << (char const*)glGetString(GL_RENDERER);
	cnote << "Extensions" << (char const*)glGetString(GL_EXTENSIONS);

//#if LIGHTBOX_USE_SDL
	// Wierd, but SDL/GL3 implementation requires that a font be drawn first.
	// Presumably some initialization I'm not doing, but can't isolate the magic call.
//	Font(ubuntu_r_ttf, 20.f).draw(fCoord(60, 60), " ");
//#endif

	LB_GL(glDisable, GL_CULL_FACE);
	LB_GL(glDisable, GL_DEPTH_TEST);
	LB_GL(glEnable, GL_BLEND);
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_joint.init(_d);
	m_fontManager.initGraphics();
	m_root->setGeometry(fRect(0, 0, _d.widthMM(), _d.heightMM()));
	m_root->initGraphicsRecursive();
	m_root->show(true);
}

void GUIApp::finiGraphics(Display&)
{
	cnote << "Finalizing GUI graphics";
	m_root->finiGraphicsRecursive();
	m_fontManager.finiGraphics();
	m_joint.fini();
	m_cache.clear();
	m_root->show(false);
}

GUIApp::ImageCache::ImageCache(iSize _ps):
	fb(Framebuffer::Create),
	tx(_ps),
	nextfree(0)
{
	FramebufferUser u(fb);
	u.attachColor(tx);
}

bool GUIApp::ImageCache::fit(iRect _g, ViewLayerPtr _v)
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
			if (int(tx.size().height() - nextfree) < _g.height())
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
		if (int(tx.size().width() - it->second.second) >= _g.width())
		{
			// yey - enough space on the row.
			iRect tr = iRect(it->second.second, it->second.first, _g.width(), _g.height());
			vs[_v].pos = tr;
			vs[_v].index = collated.size() / (6 * 4);
			fRect ftr = fRect(tr) / fSize(tx.size());
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

void GUIApp::registerAlive(View const& _v)
{
	lock_guard<mutex> l(x_alive);
	m_alive.insert(_v);
	m_joint.display->setAnimating();
}

void GUIApp::unregisterAlive(View const& _v)
{
	lock_guard<mutex> l(x_alive);
	m_alive.erase(_v);
	if (m_alive.empty())
		m_joint.display->releaseAnimating();
}

void GUIApp::iterate(Time _d)
{
	Super::iterate(_d);
	IterateEvent e;
	e.delta = _d;

	std::set<View> alive;
	{
		x_alive.lock();
		alive = m_alive;
		x_alive.unlock();
	}
	for (View const& v: alive)
		v->event(&e);
}

bool GUIApp::drawGraphics()
{
	m_root->polishLayoutRecursive();
	m_root->initGraphicsRecursive();

	bool stillDirty = false;

	vector<ViewLayerPtr> drawingViews;
	bool visibleLayoutChanged = m_root->gatherDrawers(drawingViews, 0);

	// Add in layers
	vector<ViewLayerPtr> drawingLayers;
	unsigned activeLayers = 1;
	for (unsigned l = 0; l < activeLayers; ++l)
		for (ViewLayerPtr v: drawingViews)
		{
			if (l == 0)
				activeLayers = max<unsigned>(activeLayers, v.view->m_layers.size());
			if (l < v.view->m_layers.size())
				drawingLayers.push_back(ViewLayerPtr(v.view, l));
		}

	bool fraggedRender = false;
	if (visibleLayoutChanged)
	{
		// At least one resized drawer - reset and redraw everything (in the future we might attempt a more evolutionary cache design).
		m_cache.clear();

		for (ViewLayerPtr v: drawingLayers)
		{
			v->update();
			v.view->m_visibleLayoutChanged = false;
			assert(v->globalLayer().width() <= (int)joint().display->width() && v->globalLayer().height() <= (int)joint().display->height());
			while (true)
			{
				// assuming page is valid, try to find a position in m_cache[page] and put it into pos...
				if (m_cache.size() && m_cache.back().fit(v->globalLayer(), v))
					break;
				// End of cache - add a new page onto m_cache.
				iSize ps(joint().display->width(), joint().display->height());
				m_cache.push_back(ImageCache(ps));
			}
		}
		for (auto& cache: m_cache)
		{
			cache.geom = Buffer<float>(cache.collated);
			cache.collated.clear();
		}
	}

	for (ViewLayerPtr const& vl: drawingLayers)
		if (vl->isDirty() || !vl->isShown())
			fraggedRender = true;

	int nextRendersLeft = 3;

	vector< vector<ViewLayerPtr> > renderDirect(m_cache.size());
	if (fraggedRender)
	{
		// At least one dirty drawer - set up for render-to-texture and rerender dirty parts.
		LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LB_GL(glClearColor, 0.f, 0.f, 0.f, 0.f);
		LB_GL(glEnable, GL_SCISSOR_TEST);

		unsigned cacheIndex = 0;
		for (auto& cache: m_cache)
		{
			bool willRenderToTexture = false;
			for (auto const& i: cache.vs)
			{
				ViewLayerPtr v = i.first;
				if (!v->isShown())
				{
//					cnote << "SKIP:" << v;
					renderDirect[cacheIndex] += v;
				}
				else if (v->isDirty() && (v->isReadyForCache() || v->isPremultiplied()))
				{
//					cnote << "RENDER:" << v;
					v.preDraw();
					v->setReadyForCache();
					willRenderToTexture = true;
				}
				else if (v->isDirty())
				{
//					cnote << "DIRECT:" << v;
					v.preDraw();
					renderDirect[cacheIndex] += v;
					if (nextRendersLeft > 0)
					{
						v->setReadyForCache();
						nextRendersLeft--;
					}
					stillDirty = true;
				}
//				else
//					cnote << "CACHED:" << v;
			}

			sort(renderDirect[cacheIndex].begin(), renderDirect[cacheIndex].end(), [&](ViewLayerPtr a, ViewLayerPtr b) { return cache.vs[a].index < cache.vs[b].index; });

			if (willRenderToTexture)
			{
				for (auto const& i: cache.vs)
				{
					ViewLayerPtr v = i.first;
					if (v->isDirty() && v->isReadyForCache())
					{
						RenderToTextureSlate con(cache.fb, i.second.pos, v->overdraw());
						if (v.view->m_prefill.a() > 0.f && v.layer == 0)
							con.rect(con.limits(), v.view->m_prefill);
						v.draw(con);
						if (!v.view->m_isEnabled)
							con.rect(con.limits(), Color(0.f, .5f));
						v->setDirty(false);
					}
				}
			}
			cacheIndex++;
		}
		LB_GL(glDisable, GL_SCISSOR_TEST);
	}

	// Cache up to date - now to composite.
	LB_GL(glClearColor, 0.f, 0.f, 0.f, 0);
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);
	LB_GL(glViewport, 0, 0, GUIApp::joint().display->sizePixels().w(), GUIApp::joint().display->sizePixels().h());
	joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().display->sizePixels() * vec2(1, -1);
	if (m_showCachePage > -1)
	{
		if (m_showCachePage < (int)m_cache.size())
			Slate().blit(m_cache[m_showCachePage].tx);
	}
	else
	{
		if (true || stillDirty)
		{
			// geometry is guaranteed to be in composite-draw-order.
			unsigned cacheIndex = 0;
			for (auto& cache: m_cache)
			{
				unsigned next = 0;
				for (ViewLayerPtr v: renderDirect[cacheIndex])
				{
					assert(cache.vs.count(v));
					CachePos const& cp = cache.vs[v];
					// render all before
					if (next < cp.index)
					{
						LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
						ProgramUser u(m_joint.texture);
						u.uniform("u_tex") = cache.tx;
						u.attrib("a_texCoordPosition").setData(cache.geom, 4, 0, 6 * 4 * next * sizeof(float));
						u.triangles((cp.index - next) * 6);
					}

					// draw our view directly to framebuffer.
					if (v->isShown())
					{
						LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						auto globalLayer = v->globalLayer();
						Slate c(v.view->m_globalRect, globalLayer);
///						c.offset = joint().display->fromPixels(v.view->m_globalRect).topLeft();
						LB_GL(glEnable, GL_SCISSOR_TEST);
						LB_GL(glScissor, globalLayer.x(), joint().display->sizePixels().h() - globalLayer.bottom(), globalLayer.w(), globalLayer.h());
						iRect canvas(iCoord(0, 0), globalLayer.size());
						if (v.view->m_prefill.a() > 0.f && v.layer == 0)
							c.rect(canvas, v.view->m_prefill);
						v.draw(c);
						if (!v.view->m_isEnabled && v.layer == 0)
							c.rect(canvas, Color(0.f, .5f));
						LB_GL(glDisable, GL_SCISSOR_TEST);
					}
					next = cp.index + 1;
				}

				// render from next to end.
				if (next < cache.vs.size())
				{
					LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
					ProgramUser u(m_joint.texture);
					u.uniform("u_tex") = cache.tx;
					u.attrib("a_texCoordPosition").setData(cache.geom, 4, 0, 6 * 4 * next * sizeof(float));
					u.triangles((cache.vs.size() - next) * 6);
				}
				cacheIndex++;
			}
		}
		else
		{
			LB_GL(glEnable, GL_SCISSOR_TEST);
			LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			for (auto& cache: m_cache)
			{
				vector<ViewLayerPtr> views;
				views.reserve(cache.vs.size());
				for (auto const& vp: cache.vs)
					views.push_back(vp.first);
				sort(views.begin(), views.end(), [&](ViewLayerPtr a, ViewLayerPtr b) { return cache.vs[a].index < cache.vs[b].index; });
				for (auto const& v: views)
				{
					// draw our view directly to framebuffer.
					auto globalLayer = v->globalLayer();
					Slate c(v.view->m_globalRect, globalLayer);
//					c.offset = joint().display->fromPixels(v.view->m_globalRect).topLeft();
					LB_GL(glScissor, globalLayer.x(), joint().display->sizePixels().h() - globalLayer.bottom(), globalLayer.w(), globalLayer.h());
					v.draw(c);
					iRect canvas(iCoord(0, 0), globalLayer.size());
					if (!v.view->m_isEnabled && v.layer == 0)
						c.rect(canvas, Color(0.f, .5f));
				}
			}
			LB_GL(glDisable, GL_SCISSOR_TEST);
		}
	}

#if LIGHTBOX_PROFILE || DEBUG
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	string info = textualTime(AppEngine::get()->lastDrawTime());
	iRect rr = m_root->rect();
	Slate con(rr, rr);
	iRect infoRect(rr.bottomRight() - iCoord(200, 54), iSize(190, 44));
	con.rect(infoRect.outset(3), Color(1.f, .5f));
	Font f(Metric::Pixels, 16, "ubuntu");
	iCoord p = infoRect.topLeft();
	f.draw(p, info, RGBA::Black, AtTop|AtLeft);
	cnote << info;
#if LIGHTBOX_PROFILE || DEBUG
	info = toString(g_metrics.m_useProgramCount) + "/" + toString(g_metrics.m_drawCount);
	p += iSize(0, 16);
	f.draw(p, info, RGBA::Black, AtTop|AtLeft);
	g_metrics.reset();
#endif
#endif

	return !stillDirty;
}

bool GUIApp::keyEvent(int _code, int _direction)
{
	if (_code >= 10 && _code <= 20)
		if (_direction == 1)
			m_showCachePage = _code - 10;
		else
			m_showCachePage = -1;
	else
		return false;
	m_joint.display->repaint();
	return true;
}

bool GUIApp::motionEvent(int _id, iCoord _pos, int _direction)
{
	TouchEvent* ev = nullptr;
	if (joint().display)
	{
		if (_direction > 0)
			ev = new TouchDownEvent(_id, joint().display->fromPixels(_pos), _pos);
		else if (_direction < 0)
			ev = new TouchUpEvent(_id, joint().display->fromPixels(_pos), _pos);
		else if (m_pointerLock[_id])
			ev = new TouchMoveEvent(_id, joint().display->fromPixels(_pos), _pos);
	}

	if (!ev)
		return false;

	bool ret = false;
	if (m_pointerLock[_id])
	{
		auto gp = m_pointerLock[_id]->parent()->globalPos();
		ev->m_mmLocal -= gp;
		ret = m_pointerLock[_id]->handleEvent(ev);
		ev->m_mmLocal += gp;
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
