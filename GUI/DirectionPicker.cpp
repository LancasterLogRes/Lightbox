#include <Common/Global.h>
#include "Global.h"
#include "DirectionPicker.h"
using namespace std;
using namespace Lightbox;

DirectionPickerBody::DirectionPickerBody():
	m_direction(0.5f, 0.5f),
	m_radius(0.5f, 0.5f),
	m_mode(Point)
{
}

DirectionPickerBody::~DirectionPickerBody()
{
}

bool DirectionPickerBody::event(Event* _e)
{
	return Super::event(_e);
}

bool DirectionPickerBody::draw(Context const& _c)
{
	if (m_mode == Circle || m_mode == Fill)
		_c.circle(m_direction * geometry().size(), m_radius * geometry().size(), White);

	if (m_mode == Fill)
		_c.disc(m_direction * geometry().size(), m_radius * geometry().size(), Color(1.f, .25f));

	_c.disc(m_direction * geometry().size(), geometry().size().max() / 10.f, White);

	return Super::draw(_c);
}


