#pragma once

#include <memory>
#include <Numeric.h>
#include <GLES2/gl2.h>
#include "Global.h"

namespace Lightbox
{

class Program;
class ProgramFace;

class Attrib
{
public:
	Attrib(): m_location(0) {}
	Attrib(Program const& _p, std::string const& _name);

	int location() const { return m_location; }

private:
	std::weak_ptr<ProgramFace> m_p;
	GLint m_location;
};

}
