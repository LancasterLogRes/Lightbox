#include <Common/Global.h>
#include "Global.h"
#include "Slate.h"
#include "GUIApp.h"
#include "ListView.h"
using namespace std;
using namespace Lightbox;

ListViewBody::ListViewBody(ListModelPtr const& _model):
	m_offset		(0),
	m_model			(_model),
	m_downPointer	(-1)
{
	noteItemsChanged();
}

ListViewBody::~ListViewBody()
{
}

static const float c_mmMinScrollDistance = 4;

bool ListViewBody::event(Event* _e)
{
	// TODO: Scrolling.
	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
		// Ignore second touches
		if (m_downPointer == -1)
		{
			cdebug << boolalpha << e->id << "DOWN at" << e->mmLocal() << geometry().contains(e->mmLocal());
			m_downPos = e->mmLocal();
			m_downPointer = e->id;
			m_scrollLatch = false;
			lockPointer(e->id);
			pushed();
			return true;
		}
	}
	else if (auto e = dynamic_cast<TouchUpEvent*>(_e))
	{
		cdebug << boolalpha << e->id << "UP at" << e->mmLocal() << (e->id == m_downPointer) << geometry().contains(e->mmLocal());
		if (e->id == m_downPointer)
		{
			m_downPointer = -1;
			released(geometry().contains(e->mmLocal()) && !m_scrollLatch);
		}
		return true;
	}
	else if (auto e = dynamic_cast<TouchMoveEvent*>(_e))
	{
		assert(pointerLocked(e->id));
		fSize displacement = e->mmLocal() - m_downPos;
		cdebug << boolalpha << e->id << "MOVE at" << e->mmLocal() << (e->id == m_downPointer) << geometry().contains(e->mmLocal()) << displacement << m_scrollLatch << m_scrollOffset << m_offset;
		if (!m_scrollLatch && displacement.lengthSquared() > sqr(GUIApp::joint().display->toPixels(c_mmMinScrollDistance)))
		{
			// Switch to scroll mode.
			m_scrollLatch = true;
			m_scrollOffset = m_offset;
		}
		if (m_scrollLatch)
		{
			setOffset(m_scrollOffset - displacement.h());
			scrolled(displacement);
		}
		cdebug << boolalpha << e->id << "MOVED at" << e->mmLocal() << (e->id == m_downPointer) << geometry().contains(e->mmLocal()) << displacement << m_scrollLatch << m_scrollOffset << m_offset;
	}
	else if (auto e = dynamic_cast<IterateEvent*>(_e))
	{
		if (m_downPointer == -1)
		{
			cdebug << "ITERATE of" << textualTime(e->delta);
			setAlive(physics(e->delta));
		}
	}
	return false;
}

void ListViewBody::setOffset(float _offset)
{
	if (m_offset != _offset)
	{
		m_offset = _offset;
		update(0);
		setAlive(true);
	}
}

bool ListViewBody::physics(Time _d)
{
	// Update offset state according to Time delta _d.
	float offset = m_offset;
	float maxOffset = m_totalHeight - geometry().h();
	if (offset < 0)
		offset = min<float>(0, offset + toSeconds(_d) * 1000);
	else if (offset > maxOffset)
		offset = max<float>(maxOffset, offset - toSeconds(_d) * 1000);
	if (offset == m_offset)
		return false;
	m_offset = offset;
	update(0);
	return true;
}

void ListViewBody::draw(Slate const& _slate, unsigned)
{
	if (!m_model)
		return;
	unsigned itemCount = m_model->itemCount();

	float offset = m_offset;
	float maxOffset = m_totalHeight - geometry().h();
	if (offset < 0)
		offset /= 2;
	else if (offset > maxOffset)
		offset = (offset + maxOffset) / 2;

	fCoord cursor(0, -offset);
	for (unsigned i = 0; i < itemCount; ++i)
	{
		fSize itemSize = m_model->itemSize(i);
		fRect itemRect(cursor, fSize(max(itemSize.w(), geometry().w()), itemSize.h()));
		iRect r = _slate.toPixels(itemRect);
		// boundary glow effects will be killed
		if (r.isBelow(_slate.main()))
			break;
		if (!r.isAbove(_slate.main()))
		{
			Slate subSlate = _slate.sub(r);
			subSlate.rect(subSlate.main(), Color(0.1), 0.01);
			m_model->drawItem(i, subSlate);
		}
		cursor.setY(cursor.y() + itemSize.h());
	}
}

void ListViewBody::checkHeight()
{
	m_totalHeight = 0;
	if (m_model)
	{
		unsigned itemCount = m_model->itemCount();
		for (unsigned i = 0; i < itemCount; ++i)
			m_totalHeight += m_model->itemSize(i).h();
	}
}

void ListViewBody::initGraphics()
{
	checkHeight();
}

void ListViewBody::noteItemsChanged()
{
	// TODO: fix offset if neccessary.
	checkHeight();
	update();
}

void ListViewBody::noteItemChanged(unsigned)
{
	noteItemsChanged();
}
