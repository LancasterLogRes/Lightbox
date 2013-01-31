#include "Texture2D.h"
#include "Framebuffer.h"
#include "Program.h"
using namespace std;
using namespace Lightbox;

Texture2D Texture2D::filter(Program const& _p, Texture2D _out) const
{
	Framebuffer fb(Framebuffer::Create);
	FramebufferUser fbu(fb);

	if (!_out)
		_out = Texture2D(size());
	_out.viewport();
	fbu.attachColor(_out);
	LB_GL(glClear, GL_COLOR_BUFFER_BIT);

	ProgramUser(_p).filterMerge(*this);

	return _out;
}
