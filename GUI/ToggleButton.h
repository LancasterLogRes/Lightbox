#pragma once
#include <unordered_set>
#include "BasicButton.h"

namespace Lightbox
{

class ToggleButtonBody;
typedef std::shared_ptr<ToggleButtonBody> ToggleButton;

class ToggleButtonBody: public ViewCreator<BasicButtonBody, ToggleButtonBody>
{
	friend class ViewBody;

public:
	~ToggleButtonBody();

	ToggleButton setChecked(bool _c);
	bool isChecked() const { return m_isChecked; }
	void toggle() { setChecked(!isChecked()); }

	template <class _T> ToggleButton setOnToggled(_T const& _f) { m_onToggled = _f; return view(); }
	template <class _T> ToggleButton setOnChecked(_T const& _f) { m_onChecked = _f; return view(); }

	std::function<void(ToggleButton)> onChecked() const { return m_onChecked; }
	std::function<void(ToggleButton)> onToggled() const { return m_onToggled; }

	ToggleButton setExclusiveWith(ToggleButton const& _b);
	ToggleButton getActive() const;

protected:
	ToggleButtonBody(std::string const& _text = std::string());

	virtual void draw(Context const& _c);
	virtual void toggled() { if (m_onToggled) m_onToggled(view()); if (m_isChecked && m_onChecked) m_onChecked(view()); }
	virtual void tapped() { if (m_members) setChecked(true); else toggle(); }

private:
	typedef std::set<std::weak_ptr<ToggleButtonBody>, std::owner_less<std::weak_ptr<ToggleButtonBody> > > MemberSet;
	std::shared_ptr<MemberSet> m_members;
	std::function<void(ToggleButton)> m_onToggled;
	std::function<void(ToggleButton)> m_onChecked;
	bool m_isChecked;
};

inline ToggleButton const& operator^(ToggleButton const& _a, ToggleButton const& _b) { _a->setExclusiveWith(_b); return _a; }
template <class _T> inline _T const& operator<<(_T const& _t, ToggleButton const& _a) { _a->setOnChecked(_t); return _t; }

}
