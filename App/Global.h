#pragma once

namespace lb
{

#if !defined(LIGHTBOX_CROSS)
//#define LIGHTBOX_USE_SDL 1
#endif
#if LIGHTBOX_ANDROID || LIGHTBOX_USE_XLIB
#define LIGHTBOX_USE_EGL 1
#endif

#if LIGHTBOX_ANDROID
static const unsigned s_maxPointers = 5;
#elif !defined(LIGHTBOX_CROSS)
static const unsigned s_maxPointers = 1;
#endif

}
