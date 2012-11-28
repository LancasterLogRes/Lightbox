#include <LGL.h>
#include <App/Display.h>
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

ViewManager* ViewManager::s_this = nullptr;

ViewManager::ViewManager(Display& _d)
{
	display = &_d;
	displaySize = _d.size();

	geometryBuffer = Buffer<float>({ 0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f });

	program = Program(Shader::vertex(LB_R(View_vert)), Shader::fragment(LB_R(View_frag)));

	program.tie(uniforms);
	uniforms["displaySize"] = (vec2)(fSize)_d.size();
	offsetScale = uniforms["offsetScale"];
	color = uniforms["color"];

	geometry = program.attrib("geometry");

	defaultFont = Font(ubuntu_r_ttf, 16.f);
}

ViewBody::~ViewBody()
{
}

void ViewBody::update()
{
	ViewManager::get()->display->setOneOffAnimating();
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
	auto vm = ViewManager::get();

	vm->offsetScale = fRect(m_geometry).translated(_c.offset).asVector4();
	vm->color = vec4(.4f, .5f, .6f, 1.f);

	{
		ProgramUser u(vm->program);
		vm->geometry.setData(vm->geometryBuffer, 2);
		u.triangleStrip(4);
	}
}

bool ViewBody::handleEvent(Event* _e)
{
	if (event(_e))
	{
		return true;
	}

	// TODO: alter context of _e
	// TODO: pass to children
	// TODO: restore context of _e
	return false;
}
