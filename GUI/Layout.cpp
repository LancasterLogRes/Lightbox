#include <Common/Global.h>
#include <Common/Algorithms.h>
#include "Global.h"
#include "View.h"
#include "Layout.h"
using namespace std;
using namespace lb;

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

/**
 * The actual problem could be phrased as:
 * class ElementTreeOrElement;
 * typedef std::vector<ElementTreeOrElement> ElementTree;
 * class ElementTreeOrElement : public EitherOr<Element, ElementTree> {};
 * // ElementTree is a tree of Elements represented as a vector of items, each item being either an simple layout Element (leaf) or a sub-tree.
 * void doLayout(fSize _totalSpace, ElementTree const& _elements);
 *
 * However, that exposes the tree as the data structure and creates data-interfacing issues (e.g. how to pass back the result).
 * The algorithm may be expressible through two functions; first and probably foremost one that is able to compose a set of general (i.e. simple or composite) Elements, by laying them out, into a (composite) Element.
 * Secondly one that, given a set of Elements and an area of space, is able to perform an optimal layout of the elements in that space.
 *
 * I've prototyped the Element class, however it may need altering according to the information required by the algorithm.
 * The two functions shouldn't need altering.
 */

struct Element
{
	bool xyDependent;	///< Says that one dimension is specified in terms of the other.
	float offset;		///< "c" in "y = mx + c". Used to translate between dimensions when xyDependent is true.
	float scale;		///< "m" in "y = mx + c". Used to translate between dimensions when xyDependent is true.
	float stretch;		///< Ideal ratio of area given to this element, assuming the elements' minima/maxima are unconstrained.
	fSize minimum;		///< Minimum size to make the element.
	fSize maximum;		///< Maximum size to make the element.
};
/**
 * @brief doLayout Determine optimal horizontal layout of a set of GUI elements.
 * @param _totalSpace The total space available for the layout.
 * @param _elements The properties of each GUI element to be laid out.
 * @return The set of sizes of GUI elements in the same order as @a _elements.
 */
std::vector<fSize> doLayout(fSize _totalSpace, std::vector<Element> const& _elements);
/**
 * @brief layoutToElement Express the set of elements as a horizontally-laid out Element.
 * @param _elements The set of GUI elements to be amalgamated into a parent Element.
 * @return The (composite) Element representing the set of Elements given when laid out horizontally. 'stretch' does not need to be defined.
 */
Element layoutToElement(std::vector<Element> const& _elements);

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
			ret = fSize(ret.w() + m.w(), max(ret.h(), m.h()));
		}
	return ret + dead();
}

fSize VerticalLayout::maximumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->maximumSize();
			ret = fSize(max(ret.w(), m.w()), ret.h() + m.h());
		}
	return ret + dead();
}

fSize HorizontalLayout::minimumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			ret = fSize(ret.w() + m.w(), max(ret.h(), m.h()));
		}
	return ret + dead();
}

fSize VerticalLayout::minimumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->minimumSize();
			ret = fSize(max(ret.w(), m.w()), ret.h() + m.h());
		}
	return ret + dead();
}

fSize HorizontalLayout::fit(fSize _space)
{
	_space -= dead();
	vector<float> sizes = doLayout(_space, _space.width(), m_view->children(), [](View const&, fSize s) { return s.width(); });
	unsigned i = 0;
	fSize ret = _space;
	for (auto const& c: m_view->children())
	{
		fSize s(sizes[i], _space.height());
		s = c->fit(s);
		ret.setHeight(min(ret.height(), s.height()));
		i++;
	}
	return ret;
}

fSize VerticalLayout::fit(fSize _space)
{
	_space -= dead();
	vector<float> sizes = doLayout(_space, _space.height(), m_view->children(), [](View const&, fSize s) { return s.height(); });
	unsigned i = 0;
	fSize ret = _space;
	for (auto const& c: m_view->children())
	{
		fSize s(_space.width(), sizes[i]);
		s = c->fit(s);
		ret.setWidth(min(ret.width(), s.width()));
		i++;
	}
	return ret;
}

void VerticalLayout::layout(fSize _s)
{
	_s -= dead();
	if (m_view)
	{
		vector<float> sizes = doLayout(_s, _s.height(), m_view->children(), [](View const&, fSize s) { return s.height(); });
//		cdebug << "Sum/total:" << sumOf(sizes) << _s.height();
		fCoord cursor(m_margin.left(), m_margin.top());
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			fSize s(_s.w(), *i);
			fSize rs = c->fit(s);
			c->setGeometry(fRect(cursor, rs));
			cursor.setY(cursor.y() + *i + m_spacing);
			++i;
		}
	}
}

fSize VerticalLayout::dead() const
{
	return m_margin.extra() + fSize(0, max(0, m_view ? (int)m_view->children().size() - 1 : 0) * m_spacing);
}

void HorizontalLayout::layout(fSize _s)
{
	_s -= dead();
	if (m_view)
	{
		vector<float> sizes = doLayout(_s, _s.width(), m_view->children(), [](View const&, fSize s) { return s.width(); });
		fCoord cursor(m_margin.left(), m_margin.top());
		auto i = sizes.begin();
		for (auto const& c: m_view->children())
		{
			fSize s(*i, _s.h());
			fSize rs = c->fit(s);
			c->setGeometry(fRect(cursor, rs));
			cursor.setX(cursor.x() + *i + m_spacing);
			++i;
		}
	}
}

fSize HorizontalLayout::dead() const
{
	return m_margin.extra() + fSize(max(0, m_view ? (int)m_view->children().size() - 1 : 0) * m_spacing, 0);
}

fSize OverlayLayout::maximumSize(fSize)
{
	fSize ret(0, 0);
	if (m_view)
		for (auto const& c: m_view->children())
		{
			auto m = c->maximumSize();
			ret = fSize(max(ret.w(), m.w()), max(ret.h(), m.h()));
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
			ret = fSize(max(ret.w(), m.w()), max(ret.h(), m.h()));
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
			fSize s = _s - m_margin.extra();
			c->setGeometry(fRect(fCoord(m_margin.left(), m_margin.top()), s));
		}
}

