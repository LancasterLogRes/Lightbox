#include <Common/Global.h>
#include "Global.h"
#include "Slate.h"
#include "GUIApp.h"
#include "ListView.h"
using namespace std;
using namespace Lightbox;

ListViewBody::ListViewBody(ListModelPtr const& _model)
{
	setModel(_model);
}

ListViewBody::~ListViewBody()
{
}

bool ListViewBody::event(Event* _e)
{
	static const float c_mmMinScrollDistance = 4;

	if (auto e = dynamic_cast<TouchDownEvent*>(_e))
	{
		// Ignore second touches
		if (m_downPointer == -1)
		{
//			cdebug << boolalpha << e->id << "DOWN at" << e->mmLocal() << geometry().contains(e->mmLocal());
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
//		cdebug << boolalpha << e->id << "UP at" << e->mmLocal() << (e->id == m_downPointer) << geometry().contains(e->mmLocal());
		if (e->id == m_downPointer)
		{
			m_downPointer = -1;
			released(geometry().contains(e->mmLocal()) && !m_scrollLatch, e->mmLocal() - geometry().pos());
		}
		return true;
	}
	else if (auto e = dynamic_cast<TouchMoveEvent*>(_e))
	{
		if (pointerLocked(e->id))
		{
			fSize displacement = e->mmLocal() - m_downPos;
			cdebug << boolalpha << e->id << "MOVE at" << e->mmLocal() << (e->id == m_downPointer) << geometry().contains(e->mmLocal()) << displacement << m_scrollLatch << m_scrollOffset << m_offset;
			if (!m_scrollLatch && displacement.lengthSquared() > sqr(c_mmMinScrollDistance))
			{
				// Switch to scroll mode.
				m_scrollLatch = true;
				m_scrollOffset = m_offset;
				m_inertia = 0;
			}
			if (m_scrollLatch)
			{
				float movement = m_offset - (m_scrollOffset - displacement.h());
				setOffset(m_scrollOffset - displacement.h(), true);
				scrolled(displacement);
				m_inertia = m_inertia / 2 - movement * 50;
				cdebug << "Increased inertia to" << m_inertia;
			}
//			cdebug << boolalpha << e->id << "MOVED at" << e->mmLocal() << (e->id == m_downPointer) << geometry().contains(e->mmLocal()) << displacement << m_scrollLatch << m_scrollOffset << m_offset;
		}
	}
	else if (auto e = dynamic_cast<IterateEvent*>(_e))
	{
		if (m_downPointer == -1)
		{
//			cdebug << "ITERATE of" << textualTime(e->delta);
			setAlive(physics(e->delta));
		}
		else
		{
			m_inertia /= 2;
			cdebug << "Reducing inertia by half: " << m_inertia;
		}
	}
	return false;
}

void ListViewBody::released(bool _properClick, fCoord _pos)
{
	if (_properClick)
	{
		fCoord cursor(0, -visibleOffset());
		unsigned iC = itemCount();
		for (unsigned i = 0; i < iC; ++i)
		{
			fSize iS = itemSize(i);
			fRect itemRect(cursor, fSize(max(iS.w(), geometry().w()), iS.h()));
			if (itemRect.contains(_pos))
			{
				if (!isSelected(id(i)))
					setSelected(id(i));
				else if (selectionMethod() == SelectionMethod::One && m_notifyOnReselect)
					noteSelectionChanged();
				else if (selectionMethod() == SelectionMethod::Many)
					resetSelected(id(i));
				break;
			}
			cursor.setY(cursor.y() + iS.h());
		}
	}
	update();
}

void ListViewBody::setOffset(float _offset, bool _animate)
{
	auto clamped = clamp(_offset, offsetBounds(_animate));
	if (m_offset != clamped)
	{
		if (_animate)
		{
			m_offset = _offset;
			setAlive(true);
		}
		else
		{
			m_offset = clamped;
		}
		update(0);
	}
}

void ListViewBody::selectionChanged()
{
	update();
	if (m_onSelectionChanged)
		m_onSelectionChanged(this);
}

bool ListViewBody::physics(Time _d)
{
	// Update offset state according to Time delta _d.
	float offset = m_offset;
	auto bounds = offsetBounds(false);
	const Time c_hlOuter = FromMsecs<75>::value;
	const Time c_mmOuter = FromMsecs<100>::value;

	offset += m_inertia / 10;
	m_inertia = halfLifeDecay(c_hlOuter, _d, m_inertia);
	if (m_inertia > -0.1 && m_inertia < 0.1)
		m_inertia = 0;
	if (offset < -rect().h() / 2 || offset > m_totalHeight + rect().h() / 2)
	{
		offset = clamp<float, float>(offset, -rect().h() / 2, m_totalHeight + rect().h() / 2);
		m_inertia = 0;
	}

	if (offset < bounds.first)
		offset = min<float>(bounds.first, bounds.first + halfLifeDecay(c_hlOuter, _d, offset - bounds.first) + float(_d) / c_mmOuter);
	else if (offset > bounds.second)
		offset = max<float>(bounds.second, bounds.second + halfLifeDecay(c_hlOuter, _d, offset - bounds.second) - float(_d) / c_mmOuter);
	if (offset == m_offset)
		return false;
	m_offset = offset;
	update(0);
	return true;
}

float ListViewBody::visibleOffset() const
{
	auto bounds = offsetBounds(false);
	float offset = clamp(m_offset, offsetBounds(true));
	if (offset < bounds.first)
		offset = (1.f - sqr(1.f - (offset - bounds.first) / -geometry().h())) * -geometry().h() / 2 + bounds.first;
	else if (offset > bounds.second)
		offset = (1.f - sqr(1.f - (offset - bounds.second) / geometry().h())) * geometry().h() / 2 + bounds.second;
	return offset;
}

void ListViewBody::ensureVisible(ModelId _id)
{
	float y = 0;
	unsigned iC = itemCount();
	for (unsigned i = 0; i < iC; ++i)
		if (id(i) == _id)
		{
			y += itemSize(i).h() / 2;
			if (m_offset > y - itemSize(i).h() / 2 || m_offset + geometry().height() < y + itemSize(i).h() / 2)
				setOffset(y - geometry().height() / 2, false);
			return;
		}
		else
			y += itemSize(i).h();
}

void ListViewBody::draw(Slate const& _slate, unsigned)
{
	unsigned iC = itemCount();

	int drawn = 0;
	fCoord cursor(0, -visibleOffset());
	_slate.rect(_slate.main(), Black);
	for (unsigned i = 0; i < iC; ++i)
	{
		fSize iS = itemSize(i);
		fRect itemRect(cursor, fSize(max(iS.w(), geometry().w()), iS.h()));
		iRect r = _slate.toPixels(itemRect);
		// boundary glow effects will be killed
		if (r.isBelow(_slate.main()))
			break;
		if (!r.isAbove(_slate.main()))
		{
			Slate subSlate = _slate.sub(r);
			bool s = isSelected(id(i));
			if (s)
				subSlate.rect(subSlate.main(), Color(s ? 0.075 : 0.15), s ? 0.03 : -0.01);
			drawItem(i, subSlate);
			drawn++;
		}
		cursor.setY(cursor.y() + iS.h());
	}
	cdebug << "LVB: drawn" << drawn;
}

void ListViewBody::checkHeight()
{
	m_totalHeight = 0;
	unsigned iC = itemCount();
	for (unsigned i = 0; i < iC; ++i)
		m_totalHeight += itemSize(i).h();
}

void ListViewBody::initGraphics()
{
	checkHeight();
}

void ListViewBody::modelChanged()
{
	// TODO: fix offset if necessary.
	checkHeight();
	update();
}

void ListViewBody::itemChanged(ModelId)
{
	// height changes won't work here :(
	update();
}
