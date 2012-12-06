#include <LGL.h>
#include <App/Display.h>
#include "GUIApp.h"
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

void Context::flat(fRect _r, Color _c) const
{
	auto vm = GUIApp::joint();
	vm.offsetScale = _r.translated(offset).asVector4();
	vm.color = RGBA(_c);
	ProgramUser u(vm.flat);
	vm.flatGeometry.setData(vm.unitQuad, 2);
	u.triangleStrip(4);
}

void Context::shaded(fRect _r, Color _c, float _gradient) const
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
	m_references(1),		// 1 allows it to alwys stay alive during the construction process, even if an intrusive_ptr is constructed with it. It decremented during the doCreate() method from which the constructor is always called.
	m_layout(nullptr),
	m_childIndex(0),
	m_stretch(1.f),
	m_padding(4.f, 4.f, 4.f, 4.f),
	m_isVisible(true),
	m_isEnabled(true)
{
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
		}
		if (op)
			op->noteLayoutDirty();
		if (m_parent)
			m_parent->noteLayoutDirty();
	}
}

void ViewBody::handleDraw(Context const& _c)
{
	if (m_isVisible)
	{
		draw(_c);

		Context c = _c;
		c.offset += fSize(m_geometry.topLeft());
		for (auto const& ch: m_children)
			ch->handleDraw(c);

		if (!m_isEnabled)
		{
			_c.flat(m_geometry, Color(0, 0.5f));
		}
	}
}

void ViewBody::draw(Context const& _c)
{
	_c.shaded(m_geometry, GUIApp::style().back * .25f);
}

bool ViewBody::sensesEvent(Event* _e)
{
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

	for (auto const& ch: m_children)
		if (ch->sensesEvent(_e) && ch->handleEvent(_e))
			return true;

	if (auto e = dynamic_cast<TouchEvent*>(_e))
		e->local += p;
	return false;
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
		return fSize(0, 0);
}

fCoord ViewBody::globalPos() const
{
	fCoord ret = m_geometry.pos();
	for (View p = parent(); p; p = p->parent())
		ret += p->globalPos();
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
	out << _v->minimumSize() << "  ";
	out << _v->geometry();
	cnote << out.str();
	for (auto const& c: _v->children())
		debugOut(c, _indent + "   ");
}

}
