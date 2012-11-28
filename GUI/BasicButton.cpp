#include <array>
#include <Common/Global.h>
#include <Common/Color.h>
#include "Global.h"
#include "BasicButton.h"
using namespace std;
using namespace Lightbox;

void BasicButtonBody::draw(Context _c)
{
	auto vm = ViewManager::get();

	vm->offsetScale = fRect(m_geometry).translated(_c.offset).asVector4();
	Colour col;
	if (m_isDown)
		col = Colour(.9f, .6f, .1f);
	else
		col = Colour(.2f, .3f, .4f);

	vm->color = col.asVector4();

	{
		ProgramUser u(vm->program);
		vm->geometry.setData(vm->geometryBuffer, 2);
		u.triangleStrip(4);
	}

	vm->defaultFont.draw(m_geometry.lerp(.5f, .5f) + fSize(0, -1), m_text, Colour(0.f, 0.f, 0.f, .9f));
	vm->defaultFont.draw(m_geometry.lerp(.5f, .5f), m_text, Colour(.75f));
}

bool BasicButtonBody::event(Event* _e)
{
	if (auto tde = dynamic_cast<TouchDownEvent*>(_e))
	{
		cdebug << boolalpha << tde->id << "DOWN at" << tde->local << m_geometry.contains((fCoord)tde->local);
		if (m_geometry.contains((fCoord)tde->local))
		{
			m_isDown = true;
			m_downPointer = tde->id;
			update();
		}
		return true;
	}
	else if (auto tde = dynamic_cast<TouchUpEvent*>(_e))
	{
		cdebug << boolalpha << tde->id << "UP at" << tde->local << m_isDown << (tde->id == m_downPointer) << m_geometry.contains((fCoord)tde->local);
		if (m_isDown && tde->id == m_downPointer)
		{
			if (m_geometry.contains((fCoord)tde->local) && m_onTap)
				m_onTap(this);
			m_downPointer = -1;
			m_isDown = false;
			update();
		}
		return true;
	}
	return false;
}
