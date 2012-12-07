#pragma once

#if LIGHTBOX_USE_GLES2
#include <GLES2/gl2.h>
#elif LIGHTBOX_USE_GL
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#endif
