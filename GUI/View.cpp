#include <LGL.h>
#include <App/Display.h>
#include "GUIApp.h"
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

ViewBody::~ViewBody()
{
}

void ViewBody::update()
{
	GUIApp::joint().display->setOneOffAnimating();
}

void ViewBody::setParent(View const& _p)
{
	auto us = m_this.lock();
	assert(us);
	View p = m_parent.lock();
	if (p != _p)
	{
		if (p)
			p->m_children.erase(us);
		m_parent = _p;
		if (_p)
			_p->m_children.insert(us);
	}
}

void ViewBody::handleDraw(Context const& _c)
{
	draw(_c);

	Context c = _c;
	c.offset += fSize(m_geometry.topLeft());
	for (auto const& ch: m_children)
		ch->handleDraw(c);

//	drawOverlay(_c);
}

void ViewBody::draw(Context _c)
{
	auto vm = GUIApp::joint();

	vm.offsetScale = fRect(m_geometry).translated(_c.offset).asVector4();
	vm.color = RGBA(GUIApp::style().back * .25f);

	{
		ProgramUser u(vm.program);
		vm.geometry.setData(vm.unitQuad, 2);
		u.triangleStrip(4);
	}
}

bool ViewBody::sensesEvent(Event* _e)
{
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

fCoord ViewBody::globalPos() const
{
	fCoord ret = m_geometry.pos();
	for (View p = parent(); p; p = p->parent())
		ret += p->globalPos();
	return ret;
}

void ViewBody::lockPointer(int _id)
{
	GUIApp::get()->lockPointer(_id, view());
}

void ViewBody::releasePointer(int _id)
{
	GUIApp::get()->releasePointer(_id, view());
}
