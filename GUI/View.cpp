#include <LGL.h>
#include <App/Display.h>
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

ViewBody::ViewBody():
	m_parent(nullptr),
	m_references(1),		// 1 allows it to always stay alive during the construction process, even if an intrusive_ptr is constructed with it. It decremented during the doCreate() method from which the constructor is always called.
	m_layout(nullptr),
	m_childIndex(0),
	m_stretch(1.f),
	m_padding(0, 0, 0, 0),
	m_isVisible(true),
	m_isEnabled(true)
{
	m_whileConstructing = View(this, false);
}

ViewBody::~ViewBody()
{
	delete m_layout;
	m_layout = nullptr;
	for (auto const& c: m_children)
		c->setParent(nullptr);
}

void ViewBody::update()
{
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
			m_whileConstructing.reset();
		}
		if (op)
			op->noteLayoutDirty();
		if (m_parent)
			m_parent->noteLayoutDirty();
	}

}

void ViewBody::handleDraw(Context const& _c)
{
	// Safely kill this safety measure - we're definitely out of the constructor.
	m_whileConstructing.reset();

	if (m_isVisible)
	{
		draw(_c);

		Context c = _c;
		c.offset += fSize(m_geometry.topLeft());
		for (auto const& ch: m_children)
			ch->handleDraw(c);

		if (!m_isEnabled)
			_c.rect(m_geometry, Color(0, 0.5f));
	}
}

void ViewBody::draw(Context const& _c)
{
//	_c.rect(m_geometry, GUIApp::style().back * .25f, -.1f);
}

bool ViewBody::sensesEvent(Event* _e)
{
	// Safely kill this safety measure - we're definitely out of the constructor.
	m_whileConstructing.reset();

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

fSize ViewBody::specifyMinimumSize() const
{
	if (m_layout)
		return m_layout->minimumSize();
	else
		return fSize(0.f, 0.f);
}

fSize ViewBody::specifyMaximumSize() const
{
	if (m_layout)
		return m_layout->maximumSize();
	else
		return fSize(32767.f, 32767.f);
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

void ViewBody::releasePointer(int _id)
{
	GUIApp::get()->releasePointer(_id, this);
}

namespace Lightbox
{

View operator|(View const& _a, View const& _b)
{
	if (!!_a->children().size() == !!_b->children().size())
	{
		View ret = ViewBody::create();
		_a->setParent(ret);
		_b->setParent(ret);
		return ret;
	}
	else if (_a->children().size())
	{
		_b->setParent(_a);
		return _a;
	}
	else
	{
		_a->setParent(_b);
		return _b;
	}
}

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
