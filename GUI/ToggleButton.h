#pragma once
#include <unordered_set>
#include "BasicButton.h"

namespace Lightbox
{

class ToggleButtonBody;
typedef boost::intrusive_ptr<ToggleButtonBody> ToggleButton;

class ToggleButtonBody: public ViewCreator<BasicButtonBody, ToggleButtonBody>
{
	friend class ViewBody;

public:
	~ToggleButtonBody();

	bool isChecked() const { return m_isChecked; }
	bool isExclusiveWith(ToggleButton const& _b) const { return m_members && m_members->count(_b.get()); }
	ToggleButton checkedExclusive();
	ToggleButton const& complement() const { return m_complement; }

	void setChecked(bool _c, bool _userEvent = false);
	void setExclusiveWith(ToggleButton const& _b);
	void setComplement(ToggleButton const& _b) { m_complement = _b; }
	void setOnToggled(EventHandler const& _f) { m_onToggled = _f; }
	void setOnChecked(EventHandler const& _f) { m_onChecked = _f; }

	ToggleButton withComplement(ToggleButton const& _b) { setComplement(_b); return this; }
	ToggleButton withOnToggled(EventHandler const& _f) { setOnToggled(_f); return this; }
	ToggleButton withOnChecked(EventHandler const& _f) { setOnChecked(_f); return this; }

	void toggle(bool _userEvent = false) { setChecked(!isChecked(), _userEvent); }

protected:
	ToggleButtonBody(std::string const& _text = std::string(), Color _c = White, Grouping _grouping = NoGrouping);

	virtual void draw(Context const& _c, unsigned _l);
	virtual void toggled(bool _userEvent) { if (m_onToggled && _userEvent) m_onToggled(this); if (m_isChecked && m_onChecked && _userEvent) m_onChecked(this); update(); }
	virtual void tapped() { if (m_members && !m_members->count(nullptr)) if (m_complement && m_isChecked) m_complement->setChecked(true, true); else setChecked(true, true); else toggle(true); }

private:
	typedef std::set<ToggleButtonBody*> MemberSet;	/// OK as the ~ToggleButton removes itself from its member set.
	std::shared_ptr<MemberSet> m_members;
	EventHandler m_onToggled;
	EventHandler m_onChecked;
	ToggleButton m_complement;
	bool m_isChecked;
};

inline ToggleButton const& operator^(ToggleButton const& _a, ToggleButton const& _b) { _a->setExclusiveWith(_b); return _a; }
template <class _T> inline _T const& operator<<(_T const& _t, ToggleButton const& _a) { _a->setOnChecked(_t); return _t; }

}
