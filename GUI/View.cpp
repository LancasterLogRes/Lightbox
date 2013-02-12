#include <regex>
#include <LGL.h>
#include <App/Display.h>
#include <App/AppEngine.h>
#include <Common/StreamIO.h>
#include "GUIApp.h"
#include "View.h"
using namespace std;
using namespace Lightbox;

iSize Context::toPixels(fSize _mm) const
{
	return GUIApp::joint().display->toPixels(_mm);
}

iCoord Context::toPixels(fCoord _mm) const
{
	return GUIApp::joint().display->toPixels(_mm);
}

fSize Context::toPixelsF(fSize _mm) const
{
	return GUIApp::joint().display->toPixelsF(_mm);
}

fCoord Context::toPixelsF(fCoord _mm) const
{
	return GUIApp::joint().display->toPixelsF(_mm);
}

void Context::rectOutline(iRect _inner, iMargin _outset, Color _c) const
{
	auto vm = GUIApp::joint();

	// Left bar
	iRect lb(_inner.left() - _outset.left(), _inner.top() - _outset.top(), _outset.left(), _inner.height() + _outset.extraHeight());
	iRect rb(_inner.right(), _inner.top() - _outset.top(), _outset.right(), _inner.height() + _outset.extraHeight());
	iRect tb(_inner.left(), _inner.top() - _outset.top(), _inner.width(), _outset.top());
	iRect bb(_inner.left(), _inner.bottom(), _inner.width(), _outset.bottom());

	// TODO: do it properly!
	ProgramUser u(vm.flat);
	vm.offsetScale = fRect(lb.translated(active.pos())).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
	vm.offsetScale = fRect(rb.translated(active.pos())).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
	vm.offsetScale = fRect(tb.translated(active.pos())).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
	vm.offsetScale = fRect(bb.translated(active.pos())).asVector4();
	vm.color = RGBA(_c);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::pxRect(fRect _r) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(fCoord(active.pos())).asVector4();
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::pxRect(fRect _r, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	ProgramUser u(GUIApp::joint().flat);
	pxRect(_r);
}

void Context::pxRect(fRect _r, Color _c, float _gradient) const
{
	GUIApp::joint().color = RGBA(_c);
	GUIApp::joint().gradient = _gradient;
	ProgramUser u(GUIApp::joint().shaded);
	pxRect(_r);
}

void Context::pxDisc(fEllipse _e) const
{
	auto vm = GUIApp::joint();
	fCoord c = _e.middle() + fCoord(active.pos());
	vm.offsetScale = fVector4(c.x(), c.y(), _e.rx(), _e.ry());
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Context::pxDisc(fEllipse _e, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	ProgramUser u(GUIApp::joint().flat);
	pxDisc(_e);
}

void Context::text(Font const& _f, iCoord _anchor, std::string const& _text, Color _c) const
{
	_f.draw(_anchor + active.pos(), _text, _c.toRGBA());
}

void Context::xRule(fRect _r, float _y, float _h, Color _c) const
{
	rect(_r.lerp(0, _y, 1, _y).outset(0, _h / 2), _c);
}

void Context::yRule(fRect _r, float _x, float _w, Color _c) const
{
	rect(_r.lerp(_x, 0, _x, 1).outset(_w / 2, 0), _c);
}

void Context::rect(fRect _r, Program const& _p) const
{
	fCoord c = toDevice(_r.pos());
	fSize s = pixelsF(_r.size());
	GUIApp::joint().offsetScale = fVector4(c.x(), c.y(), s.w(), s.h());
	ProgramUser u(_p);
	u.attrib("geometry").setData(GUIApp::joint().unitQuad, 2);
	u.triangleStrip(4);
}

void Context::rect(fRect _r, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	rect(_r, GUIApp::joint().flat);
}

void Context::rect(fRect _r, Color _c, float _gradient) const
{
	GUIApp::joint().color = RGBA(_c);
	GUIApp::joint().gradient = _gradient;
	rect(_r, GUIApp::joint().shaded);
}

void Context::disc(fEllipse _e, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	disc(_e, GUIApp::joint().flat);
}

void Context::disc(fEllipse _e, Program const& _p) const
{
	auto vm = GUIApp::joint();
	fCoord c = toDevice(_e.center());
	fSize s = pixelsF(_e.radii());
	vm.offsetScale = fVector4(c.x(), c.y(), s.w(), s.h());
	ProgramUser u(_p);
	_p.attrib("geometry").setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Context::circle(fEllipse _e, float _size, Color _c) const
{
	GUIApp::joint().color = RGBA(_c);
	circle(_e, _size, GUIApp::joint().flat);
}

void Context::circle(fEllipse _e, float _size, Program const& _p) const
{
	auto vm = GUIApp::joint();
	fCoord c = toDevice(_e.center());
	fSize s = pixelsF(_e.radii());
	vm.offsetScale = fVector4(c.x(), c.y(), s.w(), s.h());
	glLineWidth(_size);
	ProgramUser u(_p);
	_p.attrib("geometry").setData(vm.unitCircle72, 2, 0, 8);
	u.lineLoop(72);
}

void Context::text(Font const& _f, fCoord _anchor, std::string const& _text, Color _c) const
{
	_f.draw(_anchor + offset, _text, _c.toRGBA());
}

void Context::glowThumb(fCoord _pos, Color _c, float _overglow) const
{
	ProgramUser u(GUIApp::joint().colorize);
	u.uniform("u_amplitude") = 1.f;
	u.uniform("u_overglow") = _overglow;
	u.uniform("u_color") = (fVector4)_c.toRGBA();
	blit(GUIApp::joint().glowThumbTex, _pos - GUIApp::style().thumbDiameter);
}

void Context::glowThumb(iCoord _pos, Color _c, float _overglow) const
{
	ProgramUser u(GUIApp::joint().colorize);
	u.uniform("u_amplitude") = 1.f;
	u.uniform("u_overglow") = _overglow;
	u.uniform("u_color") = (fVector4)_c.toRGBA();
	blit(GUIApp::joint().glowThumbTex, _pos - toPixels(GUIApp::style().thumbDiameter));
}

void Context::glowRectOutline(iRect _inner, Color _col, float _overglow) const
{
	glowRect(_inner.outset(iMargin(GUIApp::joint().lightEdgePixels.w(), GUIApp::joint().lightEdgePixels.h(), 0, 0)), _col, _overglow);
}

void Context::glowRectInline(iRect _outer, Color _col, float _overglow) const
{
	glowRect(_outer.inset(iMargin(0, 0, GUIApp::joint().lightEdgePixels.w(), GUIApp::joint().lightEdgePixels.h())), _col, _overglow);
}

void Context::glowRect(iRect _upperLeftEdgeOfLight, Color _col, float _overglow) const
{
	ProgramUser u(GUIApp::joint().colorize);
	u.uniform("u_color") = (fVector4)_col.toRGBA();
	u.uniform("u_amplitude") = 5.f;
	u.uniform("u_overglow") = _overglow;
	auto glowPixels = GUIApp::joint().glowPixels;
	auto lightEdgePixels = GUIApp::joint().lightEdgePixels;
	auto glowCornerTex = GUIApp::joint().glowCornerTex;
	iSize cornerSize = glowPixels * 2 + lightEdgePixels;
	blit(iRect(cornerSize), glowCornerTex, iRect(_upperLeftEdgeOfLight.topLeft() - glowPixels, iSize(0, 0)));
	blit(iRect(cornerSize).flippedHorizontal(), glowCornerTex, iRect(_upperLeftEdgeOfLight.topRight() - glowPixels, iSize(0, 0)));
	blit(iRect(cornerSize).flippedVertical(), glowCornerTex, iRect(_upperLeftEdgeOfLight.bottomLeft() - glowPixels, iSize(0, 0)));
	blit(iRect(cornerSize).flippedHorizontal().flippedVertical(), glowCornerTex, iRect(_upperLeftEdgeOfLight.bottomRight() - glowPixels, iSize(0, 0)));
	iSize extra = _upperLeftEdgeOfLight.size() - lightEdgePixels - glowPixels * 2;
	blit(iRect(glowPixels.w() * 2 + lightEdgePixels.w(), 0, 1, glowPixels.h() * 2 + lightEdgePixels.h()), glowCornerTex, iRect(_upperLeftEdgeOfLight.x() + glowPixels.w() + lightEdgePixels.w(), _upperLeftEdgeOfLight.y() - glowPixels.w(), extra.w(), 0));
	blit(iRect(glowPixels.w() * 2 + lightEdgePixels.w(), 0, 1, glowPixels.h() * 2 + lightEdgePixels.h()).flippedVertical(), glowCornerTex, iRect(_upperLeftEdgeOfLight.x() + glowPixels.w() + lightEdgePixels.w(), _upperLeftEdgeOfLight.bottom() - glowPixels.h(), extra.w(), 0));
	blit(iRect(0, glowPixels.h() * 2 + lightEdgePixels.h(), glowPixels.w() * 2 + lightEdgePixels.w(), 1), glowCornerTex, iRect(_upperLeftEdgeOfLight.x() - glowPixels.h(), _upperLeftEdgeOfLight.y() + glowPixels.h() + lightEdgePixels.h(), 0, extra.h()));
	blit(iRect(0, glowPixels.h() * 2 + lightEdgePixels.h(), glowPixels.w() * 2 + lightEdgePixels.w(), 1).flippedHorizontal(), glowCornerTex, iRect(_upperLeftEdgeOfLight.right() - glowPixels.h(), _upperLeftEdgeOfLight.y() + glowPixels.h() + lightEdgePixels.h(), 0, extra.h()));
}

void Context::blit(iRect _src, Texture2D const& _tex, iRect _dest) const
{
	ProgramUser u(GUIApp::joint().texture, ProgramUser::AsDefault);
	if (_src.w() == 0)
		_src.setW(_tex.size().w());
	if (_src.h() == 0)
		_src.setH(_tex.size().h());
	if (_dest.w() == 0)
		_dest.setW(abs(_src.w()));
	if (_dest.h() == 0)
		_dest.setH(abs(_src.h()));

	float tw = _tex.size().w();
	float th = _tex.size().h();
	fRect o = fRect(fCoord(_dest.pos() + active.pos()), (fSize)_dest.size());
	std::array<float, 4 * 4> quad =
	{{
		// top left.
		_src.left() / tw, _src.top() / th, o.left(), o.top(),
		// top right.
		_src.right() / tw, _src.top() / th, o.right(), o.top(),
		// bottom left.
		_src.left() / tw, _src.bottom() / th, o.left(), o.bottom(),
		// bottom right.
		_src.right() / tw, _src.bottom() / th, o.right(), o.bottom()
	}};
	u.uniform("u_tex") = _tex;
	u.attrib("a_texCoordPosition").setStaticData(quad.data(), 4, 0);
	u.triangleStrip(4);
}

void Context::blit(Texture2D const& _tex, fCoord _pos) const
{
	ProgramUser u(GUIApp::joint().texture, ProgramUser::AsDefault);
	fCoord o = toDevice(_pos);
	std::array<float, 4 * 4> quad =
	{{
		// top left.
		0, 0, o.x(), o.y(),
		// top right.
		1, 0, o.x() + _tex.size().w(), o.y(),
		// bottom left.
		0, 1, o.x(), o.y() + _tex.size().h(),
		// bottom right.
		1, 1, o.x() + _tex.size().w(), o.y() + _tex.size().h()
	}};
	u.uniform("u_tex") = _tex;
	u.attrib("a_texCoordPosition").setStaticData(quad.data(), 4, 0);
	u.triangleStrip(4);
}

RenderToTextureContext::RenderToTextureContext(Texture2D const& _tex):
	Context(iRect((iCoord)_tex.size()), iRect((iCoord)_tex.size())),
	m_tex(_tex),
	m_fb(Framebuffer::Create),
	m_fbu(m_fb)
{
	m_fbu.attachColor(m_tex);
	m_fbu.checkComplete();
	m_tex.viewport();
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);

	GUIApp::joint().u_displaySize = (fVector2)(fSize)m_tex.size();
	LB_GL(glBlendFunc, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

RenderToTextureContext::~RenderToTextureContext()
{
	GUIApp::joint().u_displaySize = (fVector2)(fSize)GUIApp::joint().displaySizePixels;
	LB_GL(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Layer::refresh()
{
	if (GUIApp::get() && GUIApp::joint().display)
		GUIApp::joint().display->setOneOffAnimating();
}

ViewBody::ViewBody():
	m_parent(nullptr),
	m_references(0),
	m_constructionReference(false),
	m_layout(nullptr),
	m_childIndex(0),
	m_stretch(1.f),
	m_isShown(true),
	m_isEnabled(true),
	m_visibleLayoutChanged(true),
	m_graphicsInitialized(false),
	m_layers(1)
{
	// Allows it to always stay alive during the construction process, even if an intrusive_ptr
	// is destructed within it. This must be explicitly decremented at some point afterwards,
	// once we're sure there's another instrusive_ptr floating around (e.g. after construction
	// in the doCreate method).
	// NOTE: If you must subclass and use new directly outside of create, make sure you call
	// doneConstruction() (and store the View object it returns) sometime after the new call
	// or finalConstructor() at the end of the final constructor of the class.
	m_references = 1;
	m_constructionReference = true;
}

ViewBody::~ViewBody()
{
	delete m_layout;
	m_layout = nullptr;

	clearChildren();
}

void ViewBody::executeDraw(Context const& _c, unsigned _layer)
{
	if (m_graphicsInitialized)
		draw(_c, _layer);
}

void ViewBody::clearChildren()
{
	// Mustn't use a for loop as the iterator will become invalid as the child removes
	// itself from it in the setParent call.
	while (m_children.size())
	{
		// The child must stay valid for the duration of the call...
		View c = *m_children.begin();
		c->setParent(nullptr);
	}
}

void ViewBody::update(int _layer)
{
	if (_layer >= 0)
	{
		if (_layer < (int)m_layers.size())
			m_layers[_layer].update();
			// else graphics might not yet be initialized.
	}
	else
		for (auto& l: m_layers)
			l.update();
}

void ViewBody::setParent(View const& _p)
{
	if (m_parent != _p)
	{
		auto op = m_parent;
		if (m_parent)
			m_parent->m_children.erase(this);
		m_parent = _p.get();
		if (_p)
		{
			if (_p->m_children.size())
				m_childIndex = (*prev(_p->m_children.end()))->m_childIndex + 1;
			else
				m_childIndex = 0;
			_p->m_children.insert(this);

			// We can safely kill this safety measure now.
			finalConstructor();
		}
		if (op)
			op->noteLayoutDirty();
		if (m_parent)
			m_parent->noteLayoutDirty();
	}
}

void ViewBody::setLayers(Layers const& _l)
{
	m_layers = _l;
	m_visibleLayoutChanged = true;
	update();
}

bool ViewBody::gatherDrawers(std::vector<ViewLayerPtr>& _l, unsigned _layer, fCoord _o, bool _ancestorVisibleLayoutChanged)
{
	bool ret = m_visibleLayoutChanged;
	m_globalRectMM = m_geometry.translated(_o);
	m_globalRect = GUIApp::joint().display->toPixels(m_globalRectMM);
	for (unsigned i = 0; i < m_layers.size(); ++i)
		m_layers[i].m_globalLayer = m_globalRect.outset(m_layers[i].overdraw());

	// Visibility is inherited, so if we draw and have not had our visibility changed directly
	// but an ancestor has, we must inherit that invalidity.
	// This is particularly important if the ancestor doesn't draw, since it will go unnoticed
	// during the compositing stage otherwise.
	_ancestorVisibleLayoutChanged |= m_visibleLayoutChanged;

	if (draws() && m_isShown)
	{
		// If we're going to draw, we need to flag ourselves as visible-layout-changed if us or
		// any of our ancestors have had their layout visibly changed.
		m_visibleLayoutChanged = _ancestorVisibleLayoutChanged;
		_l += ViewLayerPtr((ViewBody*)this, 0u);
	}
	else
		// If it can't be reset by the compositor because it's not in the view tree, then we'll
		// need to reset it here; if it's visible, then its attribute may yet alter its children.
		// Otherwise, the tree traversal ends here.
		m_visibleLayoutChanged = false;

	if (m_isShown)
		for (auto const& ch: m_children)
			ret = ch->gatherDrawers(_l, _layer, m_globalRectMM.pos(), _ancestorVisibleLayoutChanged) || ret;
	return ret;
}

void ViewBody::draw(Context const&, unsigned)
{
}

bool ViewBody::sensesEvent(Event* _e)
{
	// Safely kill this safety measure - we're definitely out of the constructor.
	finalConstructor();

	if (auto e = dynamic_cast<TouchEvent*>(_e))
		return m_isShown && m_isEnabled && m_geometry.contains(e->local);
	else
		return true;
}

bool ViewBody::handleEvent(Event* _e)
{
	if (event(_e))
		return true;

	auto p = m_geometry.pos();
	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->local -= p;

	bool ret = false;
	for (auto const& ch: m_children)
		if (ch->sensesEvent(_e) && ch->handleEvent(_e))
		{
			ret = true;
			break;
		}

	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->local += p;

	return ret;
}

void ViewBody::relayout()
{
	if (m_layout)
	{
		m_layout->layout(m_geometry.size());
		update();
	}
}

fSize ViewBody::specifyMinimumSize(fSize _s) const
{
	if (m_layout)
		return m_layout->minimumSize(_s);
	else
		return fSize(0.f, 0.f);
}

fSize ViewBody::specifyMaximumSize(fSize _s) const
{
	if (m_layout)
		return m_layout->maximumSize(_s);
	else
		return _s;
}

fSize ViewBody::specifyFit(fSize _space) const
{
	if (m_layout)
		return m_layout->fit(_space);
	else
		return _space;
}

fCoord ViewBody::globalPos() const
{
	fCoord ret = geometry().pos();
	for (View p = parent(); p; p = p->parent())
		ret += p->geometry().pos();
	return ret;
}

void ViewBody::lockPointer(int _id)
{
	GUIApp::get()->lockPointer(_id, this);
}

bool ViewBody::pointerLocked(int _id)
{
	return GUIApp::get()->pointerLocked(_id, this);
}

void ViewBody::releasePointer(int _id)
{
	GUIApp::get()->releasePointer(_id, this);
}

string ViewBody::name() const
{
	string ret = demangled(typeid(*this).name());
	if (ret.substr(0, 10) == "Lightbox::")
		ret = ret.substr(10);
	if (ret.substr(ret.size() - 4) == "Body")
		ret = ret.substr(0, ret.size() - 4);
	return ret;
}

/*namespace Lightbox
{
template <class _S> _S& operator<<(_S& _out, boost::any const& _a)
{
	try	{ _out << boost::any_cast<int>(_a); return _out; } catch (...) {}
	try	{ _out << boost::any_cast<unsigned>(_a); return _out; } catch (...) {}
	try	{ _out << boost::any_cast<char>(_a); return _out; } catch (...) {}
	try	{ _out << boost::any_cast<string>(_a); return _out; } catch (...) {}
	_out << "#Unknown";
	return _out;
}
}*/

std::string Lightbox::toString(View const& _v, std::string const& _insert)
{
	std::stringstream out;
	out << (_v->m_isEnabled ? "EN" : "--") << " "
		<< (_v->m_isShown ? "VIS" : "hid") << " "
		<< (_v->m_graphicsInitialized ? "GFX " : "___ ")
		<< "*" << _v->m_layers.size() << " [";
	for (auto const& i: _v->m_layers)
		out << (i.isDirty() ? i.isShown() ? "!" : "_" : i.isShown() ? "*" : "+");
	out	<< (_v->m_visibleLayoutChanged ? " LAY" : " lay") << "] "
		<< _insert
		<< _v->name() << ": "
//		<< _v->minimumSize() << " -> "
//		<< _v->maximumSize() << "  "
		<< _v->geometry() << " (" << _v->m_children.size() << " children)";
	out << (void*)_v.get();
	for (auto const& i: _v->m_misc)
	{
		try	{ int x = boost::any_cast<int>(i.second); out << " " << i.first << "=" << x; } catch (...) {}	// i.first (string) BAD!!!
		try	{ string x = boost::any_cast<string>(i.second); out << " " << i.first << "=" << x; } catch (...) {}
	}
	return out.str();
}

void Lightbox::debugOut(View const& _v, std::string const& _indent)
{
	cnote << toString(_v, _indent);
	for (auto const& c: _v->children())
		debugOut(c, _indent + "  ");
}
