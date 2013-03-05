#pragma once

#include <memory>
#include "ListModel.h"
#include "ListModelUser.h"
#include "View.h"

namespace Lightbox
{

class ListViewBody;
typedef boost::intrusive_ptr<ListViewBody> ListView;

class ListViewBody: public ViewCreator<ViewBody, ListViewBody>, public ListModelUser
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, ListViewBody> Super;
	~ListViewBody();

	void setOnCurrentChanged(EventHandler const& _h) { m_onCurrentChanged = _h; }
	
	ListView withOnCurrentChanged(EventHandler const& _h) { setOnCurrentChanged(_h); return this; }

protected:
	explicit ListViewBody(ListModel* _model): ListViewBody(ListModelPtr(_model)) {}
	explicit ListViewBody(ListModelPtr const& _model = ListModelPtr());

	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);

	virtual void pushed() { update(); }
	virtual void scrolled(fSize) { update(); }
	virtual void released(bool _properClick, fCoord _pos);
	virtual void initGraphics();

	virtual void currentChanged(ModelId);
	virtual void itemsChanged();
	virtual void itemChanged(unsigned _i);

private:
	void setOffset(float _offset);
	bool physics(Time _d);
	void checkHeight();
	float visibleOffset() const;

	float m_offset;			// in mm
	int m_downPointer;
	fCoord m_downPos;		// in mm
	bool m_scrollLatch;
	float m_scrollOffset;	// in mm
	float m_inertia;

	// Cached info
	mutable float m_totalHeight;	// in mm

	EventHandler m_onCurrentChanged;
};

}


