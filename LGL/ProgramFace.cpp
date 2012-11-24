#include <algorithm>
#include "Global.h"
//#include "Texture.h"
#include "Program.h"
#include "ProgramFace.h"
using namespace std;
using namespace Lightbox;

shared_ptr<ProgramFace> ProgramFace::s_inUse;

ProgramFace::~ProgramFace()
{
	if (LIGHTBOX_GL_RET(glIsProgram(m_id)))
	{
		if (s_inUse.get() == this)
			drop();
		LIGHTBOX_GL(glDeleteProgram(m_id));
	}
}


void ProgramFace::link() const
{
	if (m_v && m_f && m_v.sharedPtr()->id() && m_f.sharedPtr()->id())
	{
		LIGHTBOX_GL(glAttachShader(m_id, m_v.sharedPtr()->id()));
		LIGHTBOX_GL(glAttachShader(m_id, m_f.sharedPtr()->id()));
		LIGHTBOX_GL(glLinkProgram(m_id));
		GLint linkStatus;
		LIGHTBOX_GL(glGetProgramiv(m_id, GL_LINK_STATUS, &linkStatus));
		if (linkStatus != GL_TRUE)
		{
			GLint bufLength = 0;
			LIGHTBOX_GL(glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &bufLength));
			if (bufLength)
			{
				char* buf = (char*)malloc(bufLength);
				if (buf)
				{
					LIGHTBOX_GL(glGetProgramInfoLog(m_id, bufLength, NULL, buf));
					cwarn << "Could not link program:" <<  buf;
					free(buf);
				}
			}
		}
	}
}
/*
int ProgramFace::registerSampler(Texture const& _t)
{
	auto i = find(m_reg.begin(), m_reg.end(), _t);
	if (i == m_reg.end())
	{
		_t.activate(m_reg.size());
		m_reg.push_back(_t);
		return m_reg.size() - 1;
	}
	return i - m_reg.begin();
}
*/
void ProgramFace::use(shared_ptr<ProgramFace> const& _this) const
{
	if (s_inUse)
		s_inUse->drop();
	LIGHTBOX_GL(glUseProgram(m_id));
	s_inUse = _this;
}

void ProgramFace::drop() const
{
	if (Assert(s_inUse.get() == this))
	{
		LIGHTBOX_GL(glUseProgram(0));
//		m_reg.clear();
		s_inUse = nullptr;
	}
}

