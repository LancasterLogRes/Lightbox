#pragma once

namespace Lightbox
{

#if LIGHTBOX_CROSSCOMPILATION_ANDROID
static const unsigned s_maxPointers = 5;
#elif !defined(LIGHTBOX_CROSSCOMPILATION)
static const unsigned s_maxPointers = 1;
#endif

}
