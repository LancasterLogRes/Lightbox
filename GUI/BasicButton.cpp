#include <Common/Global.h>
#include "Global.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

void BasicButtonBody::draw(Context _c)
{
	auto vm = ViewManager::get();

	vm->offsetScale = fRect(m_geometry).asVector4();
	vm->color = vec4(.4f, .5f, .6f, 1.f);

	{
		ProgramUser u(vm->program);
		vm->geometry.setData(vm->geometryBuffer, 2);
		u.triangleStrip(4);
	}

	vm->defaultFont.draw(m_geometry.lerp(.5f, .5f) + iSize(1, 1), m_text, Colour(0.f, 0.f, 0.f));
	vm->defaultFont.draw(m_geometry.lerp(.5f, .5f), m_text, Colour(1.f, 1.f, 1.f, 1.f));
}
