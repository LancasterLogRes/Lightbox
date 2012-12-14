#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "ToggleButton.h"
using namespace std;
using namespace Lightbox;

ToggleButtonBody::ToggleButtonBody(std::string const& _text):
	ViewCreator(_text),
	m_isChecked(true)
{
}

ToggleButtonBody::~ToggleButtonBody()
{
	if (m_members)
		m_members->erase(this);
}

void ToggleButtonBody::setChecked(bool _c, bool _userEvent)
{
	// only if there are members to this set - if not it stays checked.
	if (m_isChecked != _c)
	{
		m_isChecked = _c;
		if (m_isChecked && m_members)
			for (auto const& b: *m_members)
				if (b && b != this)
					b->setChecked(false, _userEvent);
		toggled(_userEvent);
	}
}

void ToggleButtonBody::setExclusiveWith(ToggleButton const& _b)
{
	if (m_members)
	{
		if (_b->m_members)
			for (auto const& b: *_b->m_members)
				if (b)
				{
					m_members->insert(b);
					b->m_members = m_members;
					b->setChecked(false);
				} else {}
		else
		{
			m_members->insert(_b.get());
			_b->m_members = m_members;
			_b->setChecked(false);
		}
	}
	else
		if (_b->m_members)
		{
			_b->m_members->insert(this);
			m_members = _b->m_members;
			setChecked(false);
		}
		else
		{
			_b->m_members = m_members = make_shared<MemberSet>();
			m_members->insert(this);
			m_members->insert(_b.get());
			_b->setChecked(false);
		}
}

ToggleButton ToggleButtonBody::checkedExclusive()
{
	if (m_members)
		for (auto const& b: *m_members)
			if (b && b->m_isChecked)
				return b;
	return (ToggleButton)this;
}

void ToggleButtonBody::draw(Context const& _c)
{
	fRect transGeo(fCoord(0, 0), geometry().size());
	_c.rect(transGeo, m_isChecked ^ isDown() ? GUIApp::style().high : GUIApp::style().back, -.1f);
	_c.text(m_isChecked ? GUIApp::style().bold : GUIApp::style().regular, transGeo.lerp(.5f, .5f), text(), RGBA::Black);
}
