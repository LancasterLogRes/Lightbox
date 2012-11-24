#include <GLES2/gl2.h>
#include "Global.h"
#include "ShaderFace.h"
using namespace Lightbox;

ShaderFace::ShaderFace(foreign_vector<uint8_t const> const& _code, bool _isFragment)
{
	m_id = LIGHTBOX_GL_RET(glCreateShader(_isFragment ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER));
	if (m_id)
	{
		GLchar const* c = (GLchar const*)_code.data();
		GLint l = _code.size();
		LIGHTBOX_GL(glShaderSource(m_id, 1, &c, &l));
		compile();
	}
}

ShaderFace::~ShaderFace()
{
	if (glIsShader(m_id))
		LIGHTBOX_GL(glDeleteShader(m_id));
}

void ShaderFace::compile()
{
	LIGHTBOX_GL(glCompileShader(m_id));
	GLint compiled;
	LIGHTBOX_GL(glGetShaderiv(m_id, GL_COMPILE_STATUS, &compiled));
	if (!compiled)
	{
		GLint infoLen = 0;
		LIGHTBOX_GL(glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &infoLen));
		if (infoLen)
		{
			char* buf = (char*) malloc(infoLen);
			if (buf)
			{
				LIGHTBOX_GL(glGetShaderInfoLog(m_id, infoLen, NULL, buf));
				cwarn << "Could not compile shader: " << buf;
				free(buf);
			}
		}
	}
}

