#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "ToggleButton.h"
using namespace std;
using namespace Lightbox;

ToggleButtonBody::ToggleButtonBody(std::string const& _text, Color _c, Grouping _grouping):
	ViewCreator(_text, _c, _grouping),
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

void ToggleButtonBody::draw(Context const& _c, unsigned _l)
{
	drawButton(_c, _l, isChecked(), isDown() || isChecked());
}
