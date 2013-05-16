#pragma once

#include <Common/Flags.h>

namespace lb
{

class ViewBody;
class Layout;

enum GroupingFlags { NoGrouping = 0, HorizontalGrouping = 1, VerticalGrouping = 2, ForceAbove = 4, ForceBelow = 8, ForceLeft = 16, ForceRight= 32 };
typedef Flags<GroupingFlags> Grouping;
LIGHTBOX_FLAGS_TYPE(GroupingFlags, Grouping);

}

#undef _A
#undef _B
#undef _C
#undef _D
#undef _E
#undef _F
#undef _G
#undef _H
#undef _I
#undef _J
#undef _K
#undef _L
#undef _M
#undef _N
#undef _O
#undef _P
#undef _Q
#undef _R
#undef _S
#undef _T
#undef _V
#undef _W
#undef _X
#undef _Y
#undef _Z
