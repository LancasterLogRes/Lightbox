#pragma once

#include <memory>
#include "View.h"

namespace Lightbox
{

class ListViewBody;
typedef boost::intrusive_ptr<ListViewBody> ListView;

class ListModel
{
public:
	virtual unsigned itemCount() = 0;
	virtual void drawItem(unsigned _i, Slate const& _s) = 0;
	virtual fSize itemSize(unsigned _i) = 0;
};

typedef std::shared_ptr<ListModel> ListModelPtr;

class ListViewBody: public ViewCreator<ViewBody, ListViewBody>
{
	friend class ViewBody;
	
public:
	typedef ViewCreator<ViewBody, ListViewBody> Super;
	~ListViewBody();

	void setModel(ListModel* _model) { m_model = ListModelPtr(_model); noteItemsChanged(); }
	void setModel(ListModelPtr const& _model) { m_model = _model; noteItemsChanged(); }
	void noteItemsChanged();
	void noteItemChanged(unsigned _i);

	void setOffset(float _offset);
	
protected:
	explicit ListViewBody(ListModel* _model): ListViewBody(ListModelPtr(_model)) {}
	explicit ListViewBody(ListModelPtr const& _model = ListModelPtr());

	virtual bool event(Event* _e);
	virtual void draw(Slate const& _c, unsigned);

	virtual void pushed() { update(); }
	virtual void scrolled(fSize) { update(); }
	virtual void released(bool _properClick) { (void)_properClick; update(); }
	virtual void initGraphics();
	
private:
	// offset is always in mm.
	float m_offset;
	float m_targetOffset;
	std::shared_ptr<ListModel> m_model;

	bool physics(Time _d);
	void checkHeight();

	float m_totalHeight;

	int m_downPointer;
	fCoord m_downPos;	// in mm
	bool m_scrollLatch;
	float m_scrollOffset;
};

}


