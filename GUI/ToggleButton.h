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

	ToggleButton setChecked(bool _c);
	bool isChecked() const { return m_isChecked; }
	void toggle() { setChecked(!isChecked()); }

	template <class _T> ToggleButton setOnToggled(_T const& _f) { m_onToggled = _f; return this; }
	template <class _T> ToggleButton setOnChecked(_T const& _f) { m_onChecked = _f; return this; }

	std::function<void(ToggleButton)> onChecked() const { return m_onChecked; }
	std::function<void(ToggleButton)> onToggled() const { return m_onToggled; }

	ToggleButton setExclusiveWith(ToggleButton const& _b);
	ToggleButton setComplement(ToggleButton const& _b) { m_complement = _b; return this; }
	bool isExclusiveWith(ToggleButton const& _b) const { return m_members && m_members->count(_b.get()); }
	ToggleButton getActive();

protected:
	ToggleButtonBody(std::string const& _text = std::string());

	virtual void draw(Context const& _c);
	virtual void toggled() { if (m_onToggled) m_onToggled(this); if (m_isChecked && m_onChecked) m_onChecked(this); }
	virtual void tapped() { if (m_members) if (m_complement && m_isChecked) m_complement->setChecked(true); else setChecked(true); else toggle(); }

private:
	typedef std::set<ToggleButtonBody*> MemberSet;	/// OK as the ~ToggleButton removes itself from its member set.
	std::shared_ptr<MemberSet> m_members;
	std::function<void(ToggleButton)> m_onToggled;
	std::function<void(ToggleButton)> m_onChecked;
	ToggleButton m_complement;
	bool m_isChecked;
};

inline ToggleButton const& operator^(ToggleButton const& _a, ToggleButton const& _b) { _a->setExclusiveWith(_b); return _a; }
template <class _T> inline _T const& operator<<(_T const& _t, ToggleButton const& _a) { _a->setOnChecked(_t); return _t; }

}
