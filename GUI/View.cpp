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
	m_padding(0, 0, 0, 0),
	m_isVisible(true),
	m_isEnabled(true),
	m_dirty(true),
	m_isCorporal(true)
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

void ViewBody::clearChildren()
{
	// Mustn't use a for loop as the iterator will become invalid as the child removes
	// itself from it in the setParent call.
	while (m_children.size())
		(*m_children.begin())->setParent(nullptr);
}

void ViewBody::update()
{
	m_dirty = true;
	m_wasDirty = false;
	GUIApp::joint().display->setOneOffAnimating();
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

void ViewBody::gatherDrawers(std::vector<ViewBody*>& _l, fCoord _o, bool _ancestorVisibleLayoutChanged)
{
	m_globalPosAsOfLastGatherDrawers = _o + geometry().pos();

	// Visibility is inherited, so if we draw and have not had our visibility changed directly
	// but an ancestor has, we must inherit that invalidity.
	// This is particularly important if the ancestor doesn't draw, since it will go unnoticed
	// during the compositing stage otherwise.
	_ancestorVisibleLayoutChanged |= m_visibleLayoutChanged;

	if (m_isCorporal && m_isVisible)
	{
		// If we're going to draw, we need to flag ourselves as visible-layout-changed if us or
		// any of our ancestors have had their layout visibly changed.
		m_visibleLayoutChanged = _ancestorVisibleLayoutChanged;
		_l += this;
	}
	else
		// If it can't be reset by the compositor because it's not in the view tree, then we'll
		// need to reset it here; if it's visible, then its attribute may yet alter its children.
		// Otherwise, the tree traversal ends here.
		m_visibleLayoutChanged = false;

	if (m_isVisible)
		for (auto const& ch: m_children)
			ch->gatherDrawers(_l, m_globalPosAsOfLastGatherDrawers, _ancestorVisibleLayoutChanged);
}

void ViewBody::draw(Context const&)
{
}

bool ViewBody::sensesEvent(Event* _e)
{
	// Safely kill this safety measure - we're definitely out of the constructor.
	finalConstructor();

	if (m_isVisible && m_isEnabled)
		if (auto e = dynamic_cast<TouchEvent*>(_e))
			return m_geometry.contains(e->local);
	return false;
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
		<< (_v->m_isVisible ? "VIS" : "hid") << " "
		<< (_v->m_isCorporal ? "DRAW" : "ndrw") << " ["
		<< (_v->m_dirty ? "DIRTY" : "clean") << " "
		<< (_v->m_visibleLayoutChanged ? "XLAYX" : " lay ") << "] "
		<< _insert
		<< _v->name() << ": "
		<< _v->minimumSize() << " -> "
		<< _v->maximumSize() << "  "
		<< _v->geometry();
	return out.str();
}

void Lightbox::debugOut(View const& _v, std::string const& _indent)
{
	cnote << toString(_v, _indent);
	for (auto const& c: _v->children())
		debugOut(c, _indent + "  ");
}
