#pragma once

#include <utility>
#include <memory>
#include <vector>
#include <set>
#include <Numeric/Coord.h>

namespace Lightbox
{

class Slate;

typedef uint64_t ModelId;
static const ModelId UnknownModelId = (ModelId)-1;

typedef std::vector<ModelId> ModelIds;
inline ModelIds modelIds(unsigned _count) { ModelIds ret(_count); for (unsigned i = 0; i < _count; ++i) ret[i] = i; return ret; }

template <class _T>
ModelIds hashedToModelIds(_T const& _m)
{
	ModelIds ret;
	std::hash<typename _T::value_type> h;
	for (auto const& i: _m)
		ret += h(i);
	return ret;
}

class ListModel
{
public:
	virtual ModelIds itemIds() = 0;
	virtual fSize itemSize(unsigned _i, ModelId _id) = 0;
	virtual void drawItem(unsigned _i, ModelId _id, Slate const& _s, bool _selected) = 0;
};

typedef std::shared_ptr<ListModel> ListModelPtr;

}
