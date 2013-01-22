#include <iostream>
#include <Common/Global.h>
#include "GL.h"
//#include "GLMap.h"
#include "Global.h"
using namespace std;

namespace Lightbox
{

void checkGlError(const char* op)
{
	for (GLint error = glGetError(); error; error = glGetError())
		cwarn << "after " << op << " glError (0x" << hex << error << ")";
}

std::string argsToString(char const* _as, int _p1)
{
	/*if ((_p1 > 100 || (strlen(_as) == 6 && _as[0] == '0' && _as[1] == 'x')) && g_symbolStrings.count(_p1))
		return g_symbolStrings.at(_p1);
	else*/ if (_as[0] == 'G' && _as[1] == 'L' && _as[2] == '_')
		return _as;
	std::stringstream ss;
	ss << _p1;
	return ss.str();
}

char const* glSymbolString(int _s)
{
/*	if (g_symbolStrings.count(_s))
		return g_symbolStrings.at(_s);
	else*/
		return nullptr;
}

}
