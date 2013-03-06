#pragma once

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <GUI/Font.h>
#include <GUI/GUIApp.h>
#include <GUI/Slate.h>
#include "ListModel.h"

namespace Lightbox
{

typedef std::pair<std::string, std::string> DoubleString;
typedef std::vector<DoubleString> DoubleStrings;

static const fSize c_indicatorRadius = fSize(10, 10);
static const float c_indicatorMargin = 8;

template <class _T>
class PairPresenter: public ListModel
{
public:
	Font const& effectiveFont() const { return m_font.isValid() ? m_font : GUIApp::style().regular; }
	Font const& effectiveSubFont() const { return m_subFont.isValid() ? m_font : GUIApp::style().small; }
	void setMargin(fMargin const& _m) { m_margin = _m; }
	void setFont(Font const& _f, Font const& _sub) { m_font = _f; m_subFont = _sub; }
	void setColor(Color _c, Color _sub) { m_color = _c; m_subColor = _sub; }
	void setSelectedColor(Color _c, Color _sub) { m_selectedColor = _c; m_selectedSubColor = _sub; }
	void setIndicator(bool _en) { m_indicator = _en; }

	void drawItem(unsigned, ModelId _id, Slate const& _s, bool _selected)
	{
		auto const& it = ((_T*)this)->_T::get(_id);
		auto inner = _s.main().inset(_s.toPixels(m_margin));
		if (m_indicator)
		{
			auto ir = _s.toPixels(c_indicatorRadius);
			inner = inner.inset(iMargin(ir.w(), 0, 0, 0));
			if (_selected)
				_s.disc(iEllipse(inner.lerp(0, 0.5), ir), m_selectedColor);
			else
				_s.circle(iEllipse(inner.lerp(0, 0.5), ir), 1, m_color);
			inner = inner.inset(iMargin(ir.w() + _s.toPixels(fSize(c_indicatorMargin, 0)).w(), 0, 0, 0));
		}
		_s.text(it.first, AtLeft|AtTop, inner.lerp(0, 0), _selected ? m_selectedColor : m_color, effectiveFont());
		_s.text(it.second, AtLeft|AtBottom, inner.lerp(0, 1), _selected ? m_selectedSubColor : m_subColor, effectiveSubFont());
	}

	fSize itemSize(unsigned, ModelId _id)
	{
		auto const& it = ((_T*)this)->_T::get(_id);
		fSize ret = effectiveFont().measure(it.first).size().vStacked(effectiveSubFont().measure(it.second).size());
		ret += m_margin.extra();
		if (m_indicator)
			ret = ret.hStacked(fSize(c_indicatorMargin, 0)).hStacked(2 * c_indicatorRadius);
		return ret;
	}

private:
	fMargin m_margin = fMargin(3, 3, 3, 3);
	Font m_font;
	Font m_subFont;
	Color m_color = Color(.5f);
	Color m_subColor = Color(.25f);
	Color m_selectedColor = Color(1.f);
	Color m_selectedSubColor = Color(.75f);
	bool m_indicator;
};

class DoubleStringsAdaptor: public PairPresenter<DoubleStringsAdaptor>
{
public:
	DoubleStringsAdaptor(DoubleStrings const* _s): m_d(_s) {}

	virtual ModelIds itemIds() { return modelIds(m_d ? m_d->size() : 0); }
	DoubleString get(ModelId _i) const { return m_d ? m_d->at(_i) : DoubleString(); }

private:
	DoubleStrings const* m_d;
};

template <class _T>
class SinglePresenter: public ListModel
{
public:
	Font const& effectiveFont() const { return m_font.isValid() ? m_font : GUIApp::style().regular; }
	void setFont(Font const& _f) { m_font = _f; }
	void setMargin(fMargin const& _m) { m_margin = _m; }
	void setColor(Color _c) { m_color = _c; }
	void setSelectedColor(Color _c) { m_selectedColor = _c; }

	void drawItem(unsigned, ModelId _id, Slate const& _s, bool _selected)
	{
		_s.text(((_T*)this)->_T::get(_id), AtCenter, _s.main().inset(_s.toPixels(m_margin)).lerp(.5f, .5f), _selected ? m_selectedColor : m_color, effectiveFont());
	}

	fSize itemSize(unsigned, ModelId _id)
	{
		return effectiveFont().measure(((_T*)this)->_T::get(_id)).size() + m_margin.extra();
	}

private:
	Font m_font;
	fMargin m_margin = fMargin(3, 3, 3, 3);
	Color m_color = Color(.5f);
	Color m_selectedColor = Color(1.f);
};

class StringsAdaptor: public SinglePresenter<StringsAdaptor>
{
public:
	StringsAdaptor(std::vector<std::string> const* _s): m_d(_s) {}

	virtual ModelIds itemIds() { return modelIds(m_d ? m_d->size() : 0); }
	std::string get(ModelId _i) const { return m_d ? m_d->at(_i) : std::string(); }

private:
	std::vector<std::string> const* m_d;
};

class StringSetAdaptor: public SinglePresenter<StringSetAdaptor>
{
public:
	StringSetAdaptor(std::set<std::string> const* _s): m_d(_s) {}

	virtual ModelIds itemIds() { return m_d ? hashedToModelIds(*m_d) : ModelIds(); }
	std::string get(ModelId _id) const { return m_d ? withHash(*m_d, _id) : std::string(); }

private:
	std::set<std::string> const* m_d;
};

template <class _T>
class VectorAdaptor: public SinglePresenter<VectorAdaptor<_T>>
{
public:
	VectorAdaptor(std::vector<_T> const* _s): m_d(_s) {}

	virtual ModelIds itemIds() { return modelIds(m_d ? m_d->size() : 0); }
	std::string get(ModelId _i) const { return m_d ? toString(m_d->at(_i)) : std::string(); }

private:
	std::vector<_T> const* m_d;
};

}


