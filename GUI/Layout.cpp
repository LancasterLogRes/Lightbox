#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "Global.h"
#include "View.h"
#include "Layout.h"
using namespace std;
using namespace Lightbox;

fSize HorizontalLayout::minimumSize()
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			auto p = c->padding();
			ret = fSize(ret.w() + m.w() + p.x() + p.z(), max(ret.h(), m.h() + p.y() + p.w()));
		}
	return ret;
}

fSize OverlayLayout::minimumSize()
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			auto p = c->padding();
			ret = fSize(max(ret.w(), m.w() + p.x() + p.z()), max(ret.h(), m.h() + p.y() + p.w()));
		}
	return ret;
}

fSize VerticalLayout::minimumSize()
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			auto p = c->padding();
			ret = fSize(max(ret.w(), m.w() + p.x() + p.z()), ret.h() + m.h() + p.y() + p.w());
		}
	return ret;
}

std::vector<float> doLayout(float _total, std::vector<float> const& _stretch, std::vector<float> const& _size)
{
	std::vector<float> ret(_stretch.size(), -1.f);
	float totalStretch = defaultTo(sumOf(_stretch), 1.f, 0.f);
	float totalUnreserved = 1.f;
	RESTART_OUTER:
	for (unsigned i = 0; i < ret.size(); ++i)
		if (ret[i] == -1.f && _total * _stretch[i] / totalStretch * totalUnreserved < _size[i])
		{
			totalStretch -= _stretch[i];
			ret[i] = _size[i] / _total;
			totalUnreserved -= ret[i];
			// reset back to start of the loop since totalUnreserved has now changed -
			// other children may need to reserve stretch space to honour their minimum.
			goto RESTART_OUTER;
		}

	// normalStretch now has entries (0, 1] for each child such that:
	//   minimum > stretch / totalStretch * totalUnreserved * totalPixels
	// in that case, normalStretch = the normalized stretch factor for the
	// child (i.e. minimum / totalPixels).
	for (unsigned i = 0; i < ret.size(); ++i)
		if (ret[i] == -1)
			ret[i] = _stretch[i] / totalStretch * totalUnreserved;

	return ret;
}

void VerticalLayout::layout(fSize _s)
{
	if (m_view)
	{
		vector<float> stretch;
		stretch.reserve(m_view->children().size());
		vector<float> minima;
		minima.reserve(m_view->children().size());
		for (auto const& c: m_view->children())
			stretch += c->stretch(),
			minima += c->minimumSize().height() + c->padding().y() + c->padding().w();
		vector<float> sizes = doLayout(_s.height(), stretch, minima);
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() - p.x() - p.z(), _s.h() * *i - p.y() - p.w());
			c->setGeometry(fRect(cursor, s).translated(fCoord(p.x(), p.y())));
			cursor.setY(cursor.y() + s.h() + p.y() + p.w());
			++i;
		}
	}
}

void OverlayLayout::layout(fSize _s)
{
	if (m_view)
	{
		fCoord cursor(0, 0);
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() - p.x() - p.z() - m_margins.x() - m_margins.z(), _s.h() - p.y() - p.w() - m_margins.w() - m_margins.y());
			c->setGeometry(fRect(cursor, s).translated(fCoord(p.x() + m_margins.x(), p.y() + m_margins.y())));
		}
	}
}

void HorizontalLayout::layout(fSize _s)
{
	if (m_view)
	{
		vector<float> stretch;
		stretch.reserve(m_view->children().size());
		vector<float> minima;
		minima.reserve(m_view->children().size());
		for (auto const& c: m_view->children())
			stretch += c->stretch(),
			minima += c->minimumSize().width() + c->padding().x() + c->padding().z();
		vector<float> sizes = doLayout(_s.width(), stretch, minima);
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() * *i - p.x() - p.z(), _s.h() - p.y() - p.w());
			c->setGeometry(fRect(cursor, s).translated(fCoord(p.x(), p.y())));
			cursor.setX(cursor.x() + s.w() + p.x() + p.z());
			++i;
		}
	}
}

