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

	// Mustn't use a for loop as the iterator will become invalid as the child removes
	// itself from it in the setParent call.
	while (m_children.size())
		(*m_children.begin())->setParent(nullptr);
}

void ViewBody::update()
{
	m_dirty = true;
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

void ViewBody::checkCache()
{
	uSize s = (uSize)geometry().size();
	if (m_cache.size() != s)
		m_cache = Texture2D(s), m_dirty = true;
	if (m_dirty)
	{
		Framebuffer fb;
		FramebufferUser u(fb);
		u.attachColor(m_cache);
		LB_GL(glViewport, 0, 0, s.w(), s.h());
		LB_GL(glClear, GL_COLOR_BUFFER_BIT);
		GUIApp::joint().u_displaySize = (fVector2)(fSize)s;
		draw(Context());
		m_dirty = false;
	}
}

void ViewBody::cleanCache()
{
	if (m_isCorporal)
		checkCache();
	for (auto const& ch: m_children)
		ch->cleanCache();
}

void ViewBody::handleDraw(Context const& _c)
{
	// Safely kill this safety measure - we're definitely out of the constructor.
	finalConstructor();

	if (m_isVisible)
	{
		Context c = _c;
		c.offset += fSize(m_geometry.topLeft());
		if (m_isCorporal)	// could be modified
			c.blit(m_cache);
		for (auto const& ch: m_children)
			ch->handleDraw(c);

		// TODO: shader for this while blitting...
//		if (!m_isEnabled)
//			_c.rect(m_geometry, Color(0, 0.5f));
	}
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

namespace Lightbox
{

void debugOut(View const& _v, std::string const& _indent)
{
	std::stringstream out;
	out << _indent;
	out << demangled(typeid(_v.get()).name()) << ": ";
	out << _v->minimumSize() << "  ->  ";
	out << _v->maximumSize() << "  ";
	out << _v->geometry();
	cnote << out.str();
	for (auto const& c: _v->children())
		debugOut(c, _indent + "   ");
}

}
