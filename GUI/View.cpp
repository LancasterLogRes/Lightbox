#include <regex>
#include <LGL.h>
#include <App/Display.h>
#include <App/AppEngine.h>
#include <Common/StreamIO.h>
#include "GUIApp.h"
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

iRect Context::pixels(ViewBody* _v) const
{
	return iRect(iCoord(0, 0), _v->m_globalRect.size());
}

iSize Context::pixels(fSize _mm) const
{
	return GUIApp::joint().display->toPixels(_mm);
}

iCoord Context::pixels(fCoord _mm) const
{
	return GUIApp::joint().display->toPixels(_mm);
}

void Context::rect(iRect _r, Color _c) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = fRect(_r.translated(active.pos())).asVector4();
	vm.color = RGBA(_c);
	ProgramUser u(vm.flat);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
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

void Context::rect(iRect _r, Color _c, float _gradient) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = fRect(_r.translated(active.pos())).asVector4();
	vm.color = RGBA(_c);
	vm.gradient = _gradient;
	ProgramUser u(vm.shaded);
	vm.shadedGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::text(Font const& _f, iCoord _anchor, std::string const& _text, RGBA _c) const
{
	_f.draw(_anchor + active.pos(), _text, _c);
}

void Context::rect(fRect _r, Color _c) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(offset).asVector4();
	vm.color = RGBA(_c);
	ProgramUser u(vm.flat);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::rect(fRect _r, Program const& _p) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(offset).asVector4();
	ProgramUser u(_p);
	_p.attrib("geometry").setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::rect(fRect _r) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(offset).asVector4();
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::disc(fCoord _center, float _r, Color _c) const
{
	auto vm = GUIApp::joint();
	fCoord c = _center + offset;
	vm.offsetScale = fVector4(c.x(), c.y(), _r, _r);
	vm.color = RGBA(_c);
	ProgramUser u(vm.flat);
	vm.flatGeometry.setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Context::disc(fCoord _center, fSize _r, Color _c) const
{
	auto vm = GUIApp::joint();
	fCoord c = _center + offset;
	vm.offsetScale = fVector4(c.x(), c.y(), _r.w(), _r.h());
	vm.color = RGBA(_c);
	ProgramUser u(vm.flat);
	vm.flatGeometry.setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Context::circle(fCoord _center, fSize _r, Color _c, float _size) const
{
	auto vm = GUIApp::joint();
	fCoord c = _center + offset;
	vm.offsetScale = fVector4(c.x(), c.y(), _r.w(), _r.h());
	vm.color = RGBA(_c);
	LB_GL(glLineWidth, _size);
	ProgramUser u(vm.flat);
	vm.flatGeometry.setData(vm.unitCircle72, 2, 0, 8);
	u.lineLoop(72);
}

void Context::disc(fCoord _center, float _r, Program const& _p) const
{
	auto vm = GUIApp::joint();
	fCoord c = _center + offset;
	vm.offsetScale = fVector4(c.x(), c.y(), _r, _r);
	ProgramUser u(_p);
	_p.attrib("geometry").setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Context::disc(fCoord _center, float _r) const
{
	auto vm = GUIApp::joint();
	fCoord c = _center + offset;
	vm.offsetScale = fVector4(c.x(), c.y(), _r, _r);
	ProgramUser u;
	u.attrib("geometry").setData(vm.unitCircle72, 2);
	u.triangleFan(74);
}

void Context::rect(fRect _r, Color _c, float _gradient) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(offset).asVector4();
	vm.color = RGBA(_c);
	vm.gradient = _gradient;
	ProgramUser u(vm.shaded);
	vm.shadedGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::text(Font const& _f, fCoord _anchor, std::string const& _text, RGBA _c) const
{
	_f.draw(_anchor + offset, _text, _c);
}

void Context::blit(Texture2D const& _tex, fCoord _pos) const
{
	ProgramUser u(GUIApp::joint().texture);
	float ox = round(_pos.x() + offset.w());
	float oy = round(_pos.y() + offset.h());
	std::array<float, 4 * 4> quad =
	{{
		// top left.
		0, 0, ox, oy,
		// top right.
		1, 0, ox + _tex.size().w(), oy,
		// bottom left.
		0, 1, ox, oy + _tex.size().h(),
		// bottom right.
		1, 1, ox + _tex.size().w(), oy + _tex.size().h()
	}};
	cnote << quad;
	u.uniform("u_tex") = _tex;
	u.attrib("a_texCoordPosition").setStaticData(quad.data(), 4, 0);
	u.triangleStrip(4);
}

ViewBody::ViewBody():
	m_parent(nullptr),
	m_references(0),
	m_layout(nullptr),
	m_childIndex(0),
	m_stretch(1.f),
	m_isShown(true),
	m_isEnabled(true),
	m_graphicsInitialized(false)
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

void ViewBody::executeDraw(Context const& _c, int _layer)
{
	if (!m_graphicsInitialized)
	{
		initGraphics();
		m_graphicsInitialized = true;
	}
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
		m_layerDirty[_layer] = true;
		m_readyForCache[_layer] = false;
	}
	else
		for (unsigned i = 0; i < m_layerDirty.size(); ++i)
		{
			m_layerDirty[i] = true;
			m_readyForCache[i] = false;
		}
	if (GUIApp::get() && GUIApp::joint().display)
		GUIApp::joint().display->setOneOffAnimating();
}

void ViewBody::setParent(View const& _p)
{
//	cnote << "(" << View(this) << ")->setParent(" << _p << ")";
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

bool ViewBody::gatherDrawers(std::vector<pair<ViewBody*, unsigned> >& _l, fCoord _o, bool _ancestorVisibleLayoutChanged)
{
	vector<iMargin> overdraw = prepareDraw();
	if (m_overdraw.size() != overdraw.size())
	{
		m_visibleLayoutChanged = true;	// actually just the overdraw that has changed so this overkill, but at least it'll work.
		m_layerDirty = vector<bool>(overdraw.size(), true);
		m_readyForCache = vector<bool>(overdraw.size(), false);
		m_globalLayer = vector<iRect>(overdraw.size());
	}
	else
		for (unsigned i = 0; i < overdraw.size(); ++i)
			if (m_overdraw[i] != overdraw[i])
				m_visibleLayoutChanged = true;	// actually just the overdraw that has changed so this overkill, but at least it'll work.
	m_overdraw = overdraw;

	bool ret = m_visibleLayoutChanged;

	m_globalRectMM = m_geometry.translated(_o);
	m_globalRect = GUIApp::joint().display->toPixels(m_globalRectMM);
	for (unsigned i = 0; i < m_overdraw.size(); ++i)
		m_globalLayer[i] = m_globalRect.outset(m_overdraw[i]);

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
		_l += make_pair((ViewBody*)this, 0u);
	}
	else
		// If it can't be reset by the compositor because it's not in the view tree, then we'll
		// need to reset it here; if it's visible, then its attribute may yet alter its children.
		// Otherwise, the tree traversal ends here.
		m_visibleLayoutChanged = false;

	if (m_isShown)
	{
		unsigned activeLayers = 1;
		for (auto const& ch: m_children)
		{
			ret = ch->gatherDrawers(_l, m_globalRectMM.pos(), _ancestorVisibleLayoutChanged) || ret;
			activeLayers = max<unsigned>(activeLayers, ch->m_overdraw.size());
		}
		for (unsigned currentLayer = 1; currentLayer < activeLayers; ++currentLayer)
			for (auto const& ch: m_children)
				if (currentLayer < ch->m_overdraw.size())
					_l += make_pair(ch.get(), currentLayer);
	}
	return ret;
}

void ViewBody::draw(Context const&)
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

std::string Lightbox::toString(View const& _v, std::string const& _insert)
{
	std::stringstream out;
	out << (_v->m_isEnabled ? "EN" : "--") << " "
		<< (_v->m_isShown ? "VIS" : "hid") << " "
		<< "*" << _v->m_overdraw.size() << " [";
	for (auto i: _v->m_layerDirty)
		out << (i ? "X " : "/");
	out	<< (_v->m_visibleLayoutChanged ? " LAY" : " lay") << "] "
		<< _insert
		<< _v->name() << ": "
		<< _v->minimumSize() << " -> "
		<< _v->maximumSize() << "  "
		<< _v->geometry() << " (" << _v->m_children.size() << " children)";
	return out.str();
}

void Lightbox::debugOut(View const& _v, std::string const& _indent)
{
	cnote << toString(_v, _indent);
	for (auto const& c: _v->children())
		debugOut(c, _indent + "  ");
}
