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
	m_fontManager.registerData(FontDefinition("Ubuntu", false), LB_R(ubuntu_r_ttf));
	m_fontManager.registerData(FontDefinition("Ubuntu", true), LB_R(ubuntu_b_ttf));

	m_root = FrameBody::create();

	m_style.fore = Color(.5f);
	m_style.back = Color(0.f);
	m_style.high = Color(1.f);
	m_style.big = Font(20, FontDefinition("Ubuntu", false));
	m_style.bigBold = Font(20, FontDefinition("Ubuntu", true));
	m_style.regular = Font(17, FontDefinition("Ubuntu", false));
	m_style.bold = Font(17, FontDefinition("Ubuntu", true));
	m_style.small = Font(13, FontDefinition("Ubuntu", false));
	m_style.smallBold = Font(13, FontDefinition("Ubuntu", true));
	m_style.thumbSize = fSize(40, 40);
	m_style.thumbOutline = 2;
	m_style.outlineColor = Black;
}

GUIApp::~GUIApp()
{
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
	m_joint = Joint();
	m_cache.clear();
	m_root->show(false);
}

GUIApp::ImageCache::ImageCache(uSize _ps):
	fb(Framebuffer::Create),
	tx(_ps),
	nextfree(0)
{
	FramebufferUser u(fb);
	u.attachColor(tx);
}

bool GUIApp::ImageCache::fit(iRect _g, ViewLayer _v)
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
			uRect tr = uRect(it->second.second, it->second.first, _g.width(), _g.height());
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

bool GUIApp::drawGraphics()
{
	bool stillDirty = false;

	vector<ViewLayer> drawers;
	bool visibleLayoutChanged = m_root->gatherDrawers(drawers, 0);

	// Add in extra layers
	unsigned firstLayerDrawers = drawers.size();
	unsigned activeLayers = 1;
	for (unsigned l = 0; l < activeLayers; ++l)
	{
		unsigned vi = 0;
		for (ViewLayer v: drawers)
			if (l == 0)
				activeLayers = max<unsigned>(activeLayers, v.view->m_overdraw.size());
			else if (l < v.view->m_overdraw.size())
			{
				drawers.push_back(ViewLayer(v.view, l));
				++vi;
				if (vi == firstLayerDrawers)
					break;
			}
	}

	if (visibleLayoutChanged)
	{
		// At least one resized drawer - reset and redraw everything (in the future we might attempt a more evolutionary cache design).
		m_cache.clear();

		for (ViewLayer v: drawers)
		{
			v.view->m_layerDirty[v.layer] = true;
			v.view->m_visibleLayoutChanged = false;
			while (true)
			{
				// assuming page is valid, try to find a position in m_cache[page] and put it into pos...
				if (m_cache.size() && m_cache.back().fit(v.view->m_globalLayer[v.layer], v))
					break;
				// End of cache - add a new page onto m_cache.
				uSize ps(joint().display->width(), joint().display->height());
				m_cache.push_back(ImageCache(ps));
			}
		}
		for (auto& cache: m_cache)
		{
			cache.geom = Buffer<float>(cache.collated);
			cache.collated.clear();
		}
	}

	vector< vector<ViewLayer> > renderToFramebuffer(m_cache.size());
	if (find_if(drawers.begin(), drawers.end(), [&](ViewLayer v){ return v.view->m_layerDirty[v.layer]; }) != drawers.end())
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
				ViewLayer v = i.first;
				if (v.view->m_layerDirty[v.layer] && v.view->m_readyForCache[v.layer])
					willRenderToTexture = true;
				else if (v.view->m_layerDirty[v.layer])
				{
					renderToFramebuffer[cacheIndex] += v;
					v.view->m_readyForCache[v.layer] = stillDirty = true;
				}
			}

			sort(renderToFramebuffer[cacheIndex].begin(), renderToFramebuffer[cacheIndex].end(), [&](ViewLayer a, ViewLayer b) { return cache.vs[a].index < cache.vs[b].index; });

			if (willRenderToTexture)
			{
				FramebufferUser u(cache.fb);
				for (auto const& i: cache.vs)
				{
					ViewLayer v = i.first;
					if (v.view->m_layerDirty[v.layer])
					{
						uRect texRect = i.second.pos;
						LB_GL(glViewport, texRect.x(), texRect.y(), texRect.w(), texRect.h());
						LB_GL(glScissor, texRect.x(), texRect.y(), texRect.w(), texRect.h());
						LB_GL(glClear, GL_COLOR_BUFFER_BIT);
						GUIApp::joint().u_displaySize = (vec2)(fSize)texRect.size();
						assert((iSize)texRect.size() == v.view->m_globalLayer[v.layer].size());
						iRect canvas(iCoord(0, 0), (iSize)texRect.size());
						Context con(canvas.inset(v.view->m_overdraw[v.layer]), canvas);
						v.view->executeDraw(con, v.layer);
						if (!v.view->m_isEnabled)
							con.rect(canvas, Color(0.f, .5f));
						v.view->m_layerDirty[v.layer] = false;
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
	{
		if (stillDirty)
		{
			// geometry is guaranteed to be in composite-draw-order.
			unsigned cacheIndex = 0;
			for (auto& cache: m_cache)
			{
				unsigned next = 0;
				for (ViewLayer v: renderToFramebuffer[cacheIndex])
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
					LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					auto globalLayer = v.view->m_globalLayer[v.layer];
					Context c(v.view->m_globalRect, globalLayer);
					c.offset = joint().display->fromPixels(v.view->m_globalRect).topLeft();
					LB_GL(glEnable, GL_SCISSOR_TEST);
					LB_GL(glScissor, globalLayer.x(), joint().display->sizePixels().h() - globalLayer.bottom(), globalLayer.w(), globalLayer.h());
					v.view->executeDraw(c, v.layer);
					iRect canvas(iCoord(0, 0), globalLayer.size());
					if (!v.view->m_isEnabled && v.layer == 0)
						c.rect(canvas, Color(0.f, .5f));
					LB_GL(glDisable, GL_SCISSOR_TEST);
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
				vector<ViewLayer> views;
				views.reserve(cache.vs.size());
				for (auto const& vp: cache.vs)
					views.push_back(vp.first);
				sort(views.begin(), views.end(), [&](ViewLayer a, ViewLayer b) { return cache.vs[a].index < cache.vs[b].index; });
				for (auto const& v: views)
				{
					// draw our view directly to framebuffer.
					auto globalLayer = v.view->m_globalLayer[v.layer];
					Context c(v.view->m_globalRect, globalLayer);
					c.offset = joint().display->fromPixels(v.view->m_globalRect).topLeft();
					LB_GL(glScissor, globalLayer.x(), joint().display->sizePixels().h() - globalLayer.bottom(), globalLayer.w(), globalLayer.h());
					v.view->executeDraw(c, v.layer);
					iRect canvas(iCoord(0, 0), globalLayer.size());
					if (!v.view->m_isEnabled && v.layer == 0)
						c.rect(canvas, Color(0.f, .5f));
				}
			}
			LB_GL(glDisable, GL_SCISSOR_TEST);
		}
	}

#if !LIGHTBOX_FINAL
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	string info = textualTime(AppEngine::get()->lastDrawTime());
	iRect rr = m_root->rect();
	Context con(rr, rr);
	iRect infoRect(rr.bottomRight() - iCoord(200, 54), iSize(190, 44));
	con.rect(infoRect.outset(3), Color(1.f, .5f));
	Font f(ByPixels, 16, "Ubuntu");
	iCoord p = infoRect.topLeft();
	f.draw(p, info, RGBA::Black, AtTop|AtLeft);
	info = toString(g_metrics.m_useProgramCount) + "/" + toString(g_metrics.m_drawCount);
	p += iSize(0, 16);
	f.draw(p, info, RGBA::Black, AtTop|AtLeft);
	g_metrics.reset();
#endif

	return !stillDirty;
}

bool GUIApp::motionEvent(int _id, iCoord _pos, int _direction)
{
	TouchEvent* ev = nullptr;
	if (_direction > 0)
		ev = new TouchDownEvent(_id, joint().display->fromPixels(_pos), _pos);
	else if (_direction < 0)
		ev = new TouchUpEvent(_id, joint().display->fromPixels(_pos), _pos);
	else if (m_pointerLock[_id])
		ev = new TouchMoveEvent(_id, joint().display->fromPixels(_pos), _pos);

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
