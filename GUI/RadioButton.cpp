#include <Common/Global.h>
#include "Global.h"
#include "GUIApp.h"
#include "RadioButton.h"
using namespace std;
using namespace Lightbox;

RadioButtonBody::RadioButtonBody(std::string const& _text):
	ViewCreator(_text),
	m_isChecked(true)
{
}

RadioButtonBody::~RadioButtonBody()
{
}

RadioButton RadioButtonBody::setChecked(bool _c)
{
	// only if there are members to this set - if not it stays checked.
	if (m_isChecked != _c)
	{
		m_isChecked = _c;
		if (m_isChecked && m_members)
			for (auto const& b: *m_members)
				if (auto bb = b.lock())
					if (&*bb != this)
						bb->setChecked(false);
		toggled();
	}
	return view();
}

RadioButton RadioButtonBody::setExclusiveWith(RadioButton const& _b)
{
	if (m_members)
	{
		if (_b->m_members)
			for (auto const& b: *_b->m_members)
				if (auto bb = b.lock())
				{
					m_members->insert(b);
					bb->m_members = m_members;
					bb->setChecked(false);
				} else {}
		else
		{
			m_members->insert(_b);
			_b->m_members = m_members;
			_b->setChecked(false);
		}
	}
	else
		if (_b->m_members)
		{
			_b->m_members->insert(view());
			m_members = _b->m_members;
			setChecked(false);
		}
		else
		{
			_b->m_members = m_members = make_shared<MemberSet>();
			m_members->insert(view());
			m_members->insert(_b);
			_b->setChecked(false);
		}
	return view();
}

RadioButton RadioButtonBody::getActive() const
{
	if (m_members)
		for (auto const& b: *m_members)
			if (auto bb = b.lock())
				if (bb->m_isChecked)
					return bb;
	return (RadioButton)view();
}

void RadioButtonBody::draw(Context _c)
{
	auto const& j = GUIApp::joint();

	auto transGeo = m_geometry.translated(_c.offset);
	j.offsetScale = transGeo.asVector4();
	j.color = m_isChecked ^ m_isDown ? GUIApp::style().high.toRGBA() : GUIApp::style().back.toRGBA();

	{
		ProgramUser u(j.program);
		j.geometry.setData(j.unitQuad, 2);
		u.triangleStrip(4);
	}

	auto const& f = m_isChecked ? GUIApp::style().bold : GUIApp::style().regular;
	f.draw(transGeo.lerp(.5f, .5f) + fSize(1, -1), m_text, RGBA(0.f, 0.f, 0.f, .9f));
	f.draw(transGeo.lerp(.5f, .5f), m_text, RGBA(GUIApp::style().fore * 2.f));
}
