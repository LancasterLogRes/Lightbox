#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>
#include "ListModel.h"

namespace lb
{

enum class SelectionMethod
{
	None,
	One,
	Many
};

typedef std::set<ModelId> ModelIdSet;

class ListModelUser
{
public:
	ModelId id(unsigned _i) const { ensureIndexed(); return _i < m_ids.size() ? m_ids[_i] : UnknownModelId; }
	int index(ModelId _id) const { ensureIndexed(); return m_rids.count(_id) ? m_rids.at(_id) : -1; }

	void clearSelection() { if (!m_selected.empty()) { m_selected.clear(); noteSelectionChanged(); } }
	bool isSelected(ModelId _id) const { return m_selected.count(_id); }
	void setSelected(ModelId _id) { if (m_selectionMethod != SelectionMethod::None && !m_selected.count(_id)) { if (m_selectionMethod == SelectionMethod::One) m_selected.clear(); m_selected.insert(_id); noteSelectionChanged(); } }
	void resetSelected(ModelId _id) { if (m_selectionMethod == SelectionMethod::Many && m_selected.count(_id)) { m_selected.erase(_id); noteSelectionChanged(); } }
	ModelId selected() const { return m_selected.size() ? *m_selected.begin() : UnknownModelId; }
	ModelIdSet selection() const { return m_selected; }
	void noteIdChanged(ModelId _new, ModelId _old) { if (m_selected.count(_old)) { m_selected.erase(_old); m_selected.insert(_new); } }

	void suspendEvents() { if (!m_suspendingEvents++) m_selectionChanged = false; }
	void releaseEvents() { if (!--m_suspendingEvents) { if (m_selectionChanged) selectionChanged(); } }

	SelectionMethod selectionMethod() const { return m_selectionMethod; }
	void setSelectionMethod(SelectionMethod _m) { m_selectionMethod = _m; clearSelection(); }

	void setModel(ListModel* _model) { m_model = ListModelPtr(_model); noteModelChanged(); }
	void setModel(ListModelPtr const& _model) { if (m_model != _model) { m_model = _model; noteModelChanged(); } }

	void noteSelectionChanged() { if (m_suspendingEvents) m_selectionChanged = true; else selectionChanged(); }
	// Called when the model itself has been changed, or when any id/index pairings have been changed (e.g. items added/removed or changing id).
	void noteModelChanged(bool _forceSelectionChanged = true) { indexDirty(); modelChanged(); clearDeadSelection(_forceSelectionChanged); }
	// Called when the basic data (not the Id) of an item has changed within the model.
	void noteItemChanged(ModelId _id) { itemChanged(_id); }

	std::shared_ptr<ListModel> const& model() const { return m_model; }

protected:
	unsigned itemCount() const { ensureIndexed(); return m_ids.size(); }
	fSize itemSize(unsigned _i) const { ensureIndexed(); return m_model ? m_model->itemSize(_i, m_ids[_i]) : fSize(); }
	void drawItem(unsigned _i, Slate const& _s) { ensureIndexed(); if (_i < m_ids.size()) m_model->drawItem(_i, m_ids[_i], _s, m_selected.count(m_ids[_i])); }

	virtual void selectionChanged() {}
	virtual void modelChanged() {}
	virtual void itemChanged(ModelId) {}

private:
	void indexDirty() { m_dirty = true; }
	void ensureIndexed() const;
	void clearDeadSelection(bool _force);

	// The data model
	std::shared_ptr<ListModel> m_model;

	// The selection model
	SelectionMethod m_selectionMethod = SelectionMethod::One;
	ModelIdSet m_selected;

	// Defered selection events
	int m_suspendingEvents = 0;
	bool m_selectionChanged = false;

	// Cached info
	mutable bool m_dirty = false;
	mutable std::vector<ModelId> m_ids;
	mutable std::map<ModelId, unsigned> m_rids;
};

}
