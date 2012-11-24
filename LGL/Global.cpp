#include <iostream>
#include <GLES2/gl2.h>
#include <Common/Global.h>
#include "Global.h"
using namespace std;

namespace Lightbox
{

void checkGlError(const char* op)
{
	for (GLint error = glGetError(); error; error = glGetError())
		cwarn << "after " << op << " glError (0x" << hex << error << ")";
}

}
