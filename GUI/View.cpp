#include <LGL.h>
#include <App/Display.h>
#include "View.h"
#include "Shaders.h"
using namespace std;
using namespace Lightbox;

ViewManager* ViewManager::s_this = nullptr;

ViewManager::ViewManager(Display& _d)
{
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

void ViewBody::setParent(View const& _p) {}

void ViewBody::draw(Context _c)
{
	auto vm = ViewManager::get();

	vm->offsetScale = fRect(m_geometry).asVector4();
	vm->color = vec4(.4f, .5f, .6f, 1.f);

	{
		ProgramUser u(vm->program);
		vm->geometry.setData(vm->geometryBuffer, 2);
		u.triangleStrip(4);
	}
}
