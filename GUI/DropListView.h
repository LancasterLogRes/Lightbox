#pragma once

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

	void setOnCurrentChanged(EventHandler const& _h) { m_onCurrentChanged = _h; }

protected:
	explicit DropListViewBody(ListModel* _model): DropListViewBody(ListModelPtr(_model)) {}
	explicit DropListViewBody(ListModelPtr const& _model = ListModelPtr());
	
	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned _l);

	virtual void tapped();
	virtual void currentChanged(ModelId) { update(); if (m_onCurrentChanged) m_onCurrentChanged(this); }

private:
	View m_back;
	ListView m_list;

	EventHandler m_onCurrentChanged;
};

}


