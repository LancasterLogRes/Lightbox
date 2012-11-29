#pragma once
#include <unordered_set>
#include "BasicButton.h"

namespace Lightbox
{

class RadioButtonBody;
typedef std::shared_ptr<RadioButtonBody> RadioButton;

class RadioButtonBody: public ViewCreator<BasicButtonBody, RadioButtonBody>
{
	friend class ViewBody;

public:
	~RadioButtonBody();

	RadioButton setChecked(bool _c);
	bool isChecked() const { return m_isChecked; }
	void toggle() { setChecked(!isChecked()); }

	template <class _T> RadioButton setOnToggled(_T const& _f) { m_onToggled = _f; return view(); }
	template <class _T> RadioButton setOnChecked(_T const& _f) { m_onChecked = _f; return view(); }

	std::function<void(RadioButton)> onChecked() const { return m_onChecked; }
	std::function<void(RadioButton)> onToggled() const { return m_onToggled; }

	RadioButton setExclusiveWith(RadioButton const& _b);
	RadioButton getActive() const;

protected:
	RadioButtonBody(std::string const& _text = std::string());

	void draw(Context _c);
	virtual void toggled() { if (m_onToggled) m_onToggled(view()); if (m_isChecked && m_onChecked) m_onChecked(view()); }
	virtual void tapped() { setChecked(true); }

private:
	typedef std::set<std::weak_ptr<RadioButtonBody>, std::owner_less<std::weak_ptr<RadioButtonBody> > > MemberSet;
	std::shared_ptr<MemberSet> m_members;
	std::function<void(RadioButton)> m_onToggled;
	std::function<void(RadioButton)> m_onChecked;
	bool m_isChecked;
};

inline RadioButton const& operator^(RadioButton const& _a, RadioButton const& _b) { _a->setExclusiveWith(_b); return _a; }
template <class _T> inline _T const& operator<<(_T const& _t, RadioButton const& _a) { _a->setOnChecked(_t); return _t; }

}
