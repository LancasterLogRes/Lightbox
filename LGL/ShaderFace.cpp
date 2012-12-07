#include "GL.h"
#include "Global.h"
#include "ShaderFace.h"
using namespace std;
using namespace Lightbox;

ShaderFace::ShaderFace(foreign_vector<uint8_t const> const& _code, bool _isFragment)
{
	m_id = LB_GL_R(glCreateShader, _isFragment ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
	if (m_id)
	{
		GLchar const* c = (GLchar const*)_code.data();
		GLint l = _code.size();
		LB_GL(glShaderSource, m_id, 1, &c, &l);
		if (!compile())
			cnote << string(c, l);
	}
}

ShaderFace::~ShaderFace()
{
	if (glIsShader(m_id))
		LB_GL(glDeleteShader, m_id);
}

bool ShaderFace::compile()
{
	LB_GL(glCompileShader, m_id);
	GLint compiled;
	LB_GL(glGetShaderiv, m_id, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		LB_GL(glGetShaderiv, m_id, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen)
		{
			char* buf = (char*) malloc(infoLen);
			if (buf)
			{
				LB_GL(glGetShaderInfoLog, m_id, infoLen, (int*)nullptr, buf);
				cwarn << "Could not compile shader: " << buf;
				free(buf);
			}
		}
		return false;
	}
	return true;
}

