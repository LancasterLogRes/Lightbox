#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "ToggleButton.h"
using namespace std;
using namespace Lightbox;

ToggleButtonBody::ToggleButtonBody(Color _c, Grouping _grouping):
	ViewCreator(_c, _grouping),
	m_isChecked(true)
{
	setDown();
	setLit();
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
	setDown(m_isChecked);
	setLit(m_isChecked);
}

void ToggleButtonBody::toggled(bool _userEvent)
{
	if (m_onToggled && _userEvent)
		m_onToggled(this);
	if (m_isChecked && m_onChecked && _userEvent)
		m_onChecked(this);
}

void ToggleButtonBody::released(bool _withFinger)
{
	if (_withFinger)
		tapped();
	else
		setDown(isLit());
}

void ToggleButtonBody::tapped()
{
	if (m_members && !m_members->count(nullptr))
		if (m_complement && m_isChecked)
			m_complement->setChecked(true, true);
		else
			setChecked(true, true);
	else
		toggle(true);
}

void ToggleButtonBody::setExclusiveWith(ToggleButton const& _b)
{
	if (m_members)
	{
		if (_b && _b->m_members)
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
			if (_b)
			{
				_b->m_members = m_members;
				_b->setChecked(false);
			}
		}
	}
	else if (_b)
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
	else
	{
		m_members = make_shared<MemberSet>();
		m_members->insert(this);
		m_members->insert(_b.get());
	}
}

ToggleButton ToggleButtonBody::checkedExclusive()
{
	if (m_members)
	{
		for (auto const& b: *m_members)
			if (b && b->m_isChecked)
				return b;
		if (m_members->count(nullptr))
			return ToggleButton();
	}
	return ToggleButton(this);
}
