#pragma once

#include <map>
#include <memory>
#include <vector>
#include "ListModel.h"

namespace Lightbox
{

class ListModelUser
{
public:
	ModelId id(unsigned _i) const { ensureIndexed(); return _i < m_ids.size() ? m_ids[_i] : UnknownModelId; }
	int index(ModelId _id) const { ensureIndexed(); return m_rids.count(_id) ? m_rids.at(_id) : -1; }

	ModelId currentId() const { return m_currentId; }
	void setCurrentId(ModelId _id) { m_currentId = _id; currentChanged(m_currentId); }

	void setModel(ListModel* _model) { m_model = ListModelPtr(_model); noteModelChanged(); }
	void setModel(ListModelPtr const& _model) { m_model = _model; noteModelChanged(); }

	// Called when the model itself has been changed, or when any id/index pairings have been changed (e.g. items added/removed or changing id).
	void noteModelChanged() { indexDirty(); modelChanged(); }
	// Called when the basic data (not the Id) of an item has changed within the model.
	void noteItemChanged(ModelId _id) { itemChanged(_id); }

	std::shared_ptr<ListModel> const& model() const { return m_model; }

protected:
	unsigned itemCount() const { ensureIndexed(); return m_ids.size(); }
	fSize itemSize(unsigned _i) const { ensureIndexed(); return m_model ? m_model->itemSize(_i, m_ids[_i]) : fSize(); }
	void drawItem(unsigned _i, Slate const& _s) { ensureIndexed(); if (_i < m_ids.size()) m_model->drawItem(_i, m_ids[_i], _s, m_currentId == m_ids[_i]); }

	virtual void currentChanged(ModelId) {}
	virtual void modelChanged() {}
	virtual void itemChanged(ModelId) {}

private:
	void indexDirty() { m_dirty = true; }
	void ensureIndexed() const;

	// The data model
	std::shared_ptr<ListModel> m_model;

	// The selection model
	ModelId m_currentId = UnknownModelId;

	// Cached info
	mutable bool m_dirty = false;
	mutable std::vector<ModelId> m_ids;
	mutable std::map<ModelId, unsigned> m_rids;
};

}
