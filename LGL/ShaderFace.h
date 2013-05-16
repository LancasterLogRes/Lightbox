#pragma once

#include <Common/Global.h>
#include "Global.h"

namespace lb
{

class ShaderFace
{
public:
	ShaderFace(foreign_vector<uint8_t const> const& _code, bool _isFragment);
	~ShaderFace();

	bool compile();
	GLenum id() const { return m_id; }

protected:
	GLuint m_id;
};

}
