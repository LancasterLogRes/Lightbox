#include <algorithm>
#include "Global.h"
#include "Texture2D.h"
#include "Program.h"
#include "ProgramFace.h"
using namespace std;
using namespace Lightbox;

shared_ptr<ProgramFace> ProgramFace::s_inUse;

ProgramFace::~ProgramFace()
{
	if (LB_GL_R(glIsProgram, m_id))
	{
		if (s_inUse.get() == this)
			drop();
		LB_GL(glDeleteProgram, m_id);
	}
}

void ProgramFace::link() const
{
	if (m_v && m_f && m_v.sharedPtr()->id() && m_f.sharedPtr()->id())
	{
		LB_GL(glAttachShader, m_id, m_v.sharedPtr()->id());
		LB_GL(glAttachShader, m_id, m_f.sharedPtr()->id());
		LB_GL(glLinkProgram, m_id);
		GLint linkStatus;
		LB_GL(glGetProgramiv, m_id, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint bufLength = 0;
			LB_GL(glGetProgramiv, m_id, GL_INFO_LOG_LENGTH, &bufLength);
			if (bufLength)
			{
				char* buf = (char*)malloc(bufLength);
				if (buf)
				{
					LB_GL(glGetProgramInfoLog, m_id, bufLength, (int*)nullptr, buf);
					cwarn << "Could not link program:" <<  buf;
					free(buf);
				}
			}
		}
	}
}

int ProgramFace::registerSampler(Texture2D const& _t)
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

static GLuint s_idInUse = 0;

void ProgramFace::use(shared_ptr<ProgramFace> const& _this) const
{
	if (s_inUse)
		s_inUse->drop();

	if (s_idInUse != m_id)
	{
		LB_GL(glUseProgram, m_id);
		s_idInUse = m_id;
	}

	s_inUse = _this;

	for (auto& p: m_uniformPages)
		p->setup(_this);

	for (auto& f: m_nv)
		f.second();
}

void ProgramFace::drop() const
{
	if (Assert(s_inUse.get() == this))
	{
//		LB_GL(glUseProgram, 0);
		m_reg.clear();
		s_inUse = nullptr;
	}
}

void ProgramFace::tie(UniformPage const& _p)
{
	_p.sharedPtr()->registerProgram(this);
	m_uniformPages.insert(_p.sharedPtr());
}

void ProgramFace::untie(UniformPage const& _p)
{
	_p.sharedPtr()->unregisterProgram(this);
	m_uniformPages.erase(_p.sharedPtr());
}

