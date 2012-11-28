#pragma once

#include <cstdint>
#include <Numeric/Vector.h>

namespace Lightbox
{

class RGBA: public fVector4
{
public:
	RGBA(float _gray = 0.f, float _a = 1.f): fVector4(_gray, _gray, _gray, _a) {}
	RGBA(float _r, float _g, float _b, float _a = 1.f): fVector4(_r, _g, _b, _a) {}
	RGBA(RGBA const& _c, float _a): fVector4(_c.m_x, _c.m_y, _c.m_z, _a) {}

	operator float const*() const { return reinterpret_cast<float const*>(this); }
	
	float r() const { return m_x; }
	float g() const { return m_y; }
	float b() const { return m_z; }
	float a() const { return m_w; }

	void setR(float _r) { m_x = _r; }
	void setG(float _g) { m_y = _g; }
	void setB(float _b) { m_z = _b; }
	void setA(float _a) { m_w = _a; }

	RGBA operator+(RGBA const& _c) const { return RGBA(m_x + _c.m_x, m_y + _c.m_y, m_z + _c.m_z, m_w + _c.m_w); }
	RGBA operator*(float _f) const { return RGBA(m_x * _f, m_y * _f, m_z * _f, m_w); }
	RGBA operator/(float _f) const { return RGBA(m_x / _f, m_y / _f, m_z / _f, m_w); }
	
	void writeRgba(uint8_t* _dest) const { _dest[0] = uint8_t(m_x * 255.f); _dest[1] = uint8_t(m_y * 255.f); _dest[2] = uint8_t(m_z * 255.f); _dest[3] = uint8_t(m_w * 255.f); }
	
	static const RGBA Black;
	static const RGBA White;
	static const RGBA Red;
	static const RGBA Orange;
	static const RGBA Yellow;
	static const RGBA Green;
	static const RGBA Blue;
	static const RGBA Indigo;
	static const RGBA Violet;
};

}
