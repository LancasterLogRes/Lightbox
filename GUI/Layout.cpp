#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "Global.h"
#include "View.h"
#include "Layout.h"
using namespace std;
using namespace Lightbox;

std::vector<float> doLayout(float _total, std::vector<float> const& _stretch, std::vector<float> const& _minima, std::vector<float> const& _maxima)
{
	std::vector<float> ret(_stretch.size(), -1.f);
	float totalStretch = defaultTo(sumOf(_stretch), 1.f, 0.f);
	float totalUnreserved = 1.f;
	RESTART_OUTER:
	for (unsigned i = 0; i < ret.size(); ++i)
		if (ret[i] == -1.f && _total * _stretch[i] / totalStretch * totalUnreserved < _minima[i])
		{
			totalStretch -= _stretch[i];
			ret[i] = _minima[i];
			totalUnreserved -= ret[i] / _total;
			// reset back to start of the loop since totalUnreserved has now changed -
			// other children may need to reserve stretch space to honour their minimum.
			goto RESTART_OUTER;
		}
		else if (ret[i] == -1.f && _total * _stretch[i] / totalStretch * totalUnreserved > _maxima[i])
		{
			totalStretch -= _stretch[i];
			ret[i] = _maxima[i];
			totalUnreserved -= ret[i] / _total;
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
			ret[i] = _stretch[i] / totalStretch * totalUnreserved * _total;

	return ret;
}

template <class _Children, class _T> vector<float> doLayout(fSize _totalSize, float _total, _Children const& _ch, _T const& _spaceToSize)
{
	vector<float> stretch;
	stretch.reserve(_ch.size());
	vector<float> minima;
	minima.reserve(_ch.size());
	vector<float> maxima;
	maxima.reserve(_ch.size());
	for (auto const& c: _ch)
		stretch += c->stretch(),
		minima += _spaceToSize(c, c->minimumSize(_totalSize)),
		maxima += _spaceToSize(c, c->maximumSize(_totalSize));
	return doLayout(_total, stretch, minima, maxima);
}

fSize HorizontalLayout::maximumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->maximumSize();
			auto p = c->padding();
			ret = fSize(ret.w() + m.w() + p.x() + p.z(), max(ret.h(), m.h() + p.y() + p.w()));
		}
	return ret;
}

fSize VerticalLayout::maximumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->maximumSize();
			auto p = c->padding();
			ret = fSize(max(ret.w(), m.w() + p.x() + p.z()), ret.h() + m.h() + p.y() + p.w());
		}
	return ret;
}

fSize HorizontalLayout::minimumSize(fSize)
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

fSize VerticalLayout::minimumSize(fSize)
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

fSize HorizontalLayout::fit(fSize _space)
{
	vector<float> sizes = doLayout(_space, _space.width(), m_view->children(), [](View const& c, fSize s) { return s.width() + c->padding().x() + c->padding().z(); });
	unsigned i = 0;
	fSize ret = _space;
	for (auto const& c: m_view->children())
	{
		auto p = c->padding();
		fSize s(sizes[i] - p.x() - p.z(), _space.height() - p.y() - p.w());
		s = c->fit(s);
		ret.setHeight(min(ret.height(), s.height() + p.y() + p.w()));
		i++;
	}
	return ret;
}

fSize VerticalLayout::fit(fSize _space)
{
	vector<float> sizes = doLayout(_space, _space.height(), m_view->children(), [](View const& c, fSize s) { return s.height() + c->padding().y() + c->padding().w(); });
	unsigned i = 0;
	fSize ret = _space;
	for (auto const& c: m_view->children())
	{
		auto p = c->padding();
		fSize s(_space.width() - p.x() - p.z(), sizes[i] - p.y() - p.w());
		s = c->fit(s);
		ret.setWidth(min(ret.width(), s.width() + p.x() + p.z()));
		i++;
	}
	return ret;
}

void VerticalLayout::layout(fSize _s)
{
	if (m_view)
	{
		vector<float> sizes = doLayout(_s, _s.height(), m_view->children(), [](View const& c, fSize s) { return s.height() + c->padding().y() + c->padding().w(); });
//		cdebug << "Sum/total:" << sumOf(sizes) << _s.height();
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() - p.x() - p.z(), *i - p.y() - p.w());
			fSize rs = c->fit(s);
			c->setGeometry(fRect(cursor, rs).translated(fCoord(p.x(), p.y())));
			cursor.setY(cursor.y() + *i);
			++i;
		}
	}
}

void HorizontalLayout::layout(fSize _s)
{
	if (m_view)
	{
		vector<float> sizes = doLayout(_s, _s.width(), m_view->children(), [](View const& c, fSize s) { return s.width() + c->padding().x() + c->padding().z(); });
		fCoord cursor(0, 0);
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(*i - p.x() - p.z(), _s.h() - p.y() - p.w());
			fSize rs = c->fit(s);
			c->setGeometry(fRect(cursor, rs).translated(fCoord(p.x(), p.y())));
			cursor.setX(cursor.x() + *i);
			++i;
		}
	}
}

fSize OverlayLayout::maximumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->maximumSize();
			auto p = c->padding();
			ret = fSize(max(ret.w(), m.w() + p.x() + p.z()), max(ret.h(), m.h() + p.y() + p.w()));
		}
	return ret;
}

fSize OverlayLayout::minimumSize(fSize)
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

fSize OverlayLayout::fit(fSize _space)
{
	return _space;
}

void OverlayLayout::layout(fSize _s)
{
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto p = c->padding();
			fSize s(_s.w() - p.x() - p.z() - m_margins.x() - m_margins.z(), _s.h() - p.y() - p.w() - m_margins.w() - m_margins.y());
			c->setGeometry(fRect(fCoord(p.x() + m_margins.x(), p.y() + m_margins.y()), s));
		}
}

