#pragma once

#include "SensitiveView.h"
#include "Frame.h"
#include "ListView.h"
#include "BasicButton.h"

namespace Lightbox
{

class DropListViewBody;
typedef boost::intrusive_ptr<DropListViewBody> DropListView;

class DropListViewBody: public ViewCreator<BasicButtonBody, DropListViewBody>, public ListModelUser
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<BasicButtonBody, DropListViewBody> Super;
	~DropListViewBody();

	void setOnSelectionChanged(EventHandler const& _h) { m_onSelectionChanged = _h; }
	DropListView withOnSelectionChanged(EventHandler const& _h) { setOnSelectionChanged(_h); return this; }

protected:
	explicit DropListViewBody(ListModel* _model): DropListViewBody(ListModelPtr(_model)) {}
	explicit DropListViewBody(ListModelPtr const& _model = ListModelPtr());
	
	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned _l);

	virtual void tapped();
	virtual void selectionChanged() { update(); if (m_onSelectionChanged) m_onSelectionChanged(this); }

private:
	SensitiveView m_back;
	ListView m_list;

	EventHandler m_onSelectionChanged;
};

}


