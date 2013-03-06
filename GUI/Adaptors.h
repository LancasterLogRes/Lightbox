#pragma once

#include <string>
#include <utility>
#include <vector>
#include <set>
#include <GUI/Font.h>
#include <GUI/GUIApp.h>
#include "ListModel.h"

namespace Lightbox
{

typedef std::pair<std::string, std::string> DoubleString;
typedef std::vector<DoubleString> DoubleStrings;

template <class _T>
class PairPresenter: public ListModel
{
public:
	void drawItem(unsigned, ModelId _id, Slate const& _s, bool _selected)
	{
		auto const& it = ((_T*)this)->_T::get(_id);
		_s.text(it.first, AtLeft|AtTop, _s.main().inset(_s.toPixels(fSize(3, 3))).lerp(0, 0), Color(_selected ? 1.f : .5f), GUIApp::style().regular);
		_s.text(it.second, AtLeft|AtBottom, _s.main().inset(_s.toPixels(fSize(3, 3))).lerp(0, 1), Color(_selected ? .75f : .25f), GUIApp::style().small);
	}

	fSize itemSize(unsigned, ModelId _id)
	{
		auto const& it = ((_T*)this)->_T::get(_id);
		return GUIApp::style().regular.measure(it.first).size().vStacked(GUIApp::style().small.measure(it.second).size()) + fSize(6, 6);
	}
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


