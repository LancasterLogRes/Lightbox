#pragma once

#include <cstdint>
#include <Numeric/Vector.h>

namespace Lightbox
{

class Colour
{
public:
	Colour(float _gray = 0.f, float _a = 1.f): m_r(_gray), m_g(_gray), m_b(_gray), m_a(_a) {}
	Colour(float _r, float _g, float _b, float _a = 1.f): m_r(_r), m_g(_g), m_b(_b), m_a(_a) {}
	Colour(Colour const& _c, float _a): m_r(_c.m_r), m_g(_c.m_g), m_b(_c.m_b), m_a(_a) {}

	fVector4 const& asVector4() const { return (fVector4 const&)*this; }
	operator float const*() const { return reinterpret_cast<float const*>(this); }
	
	float r() const { return m_r; }
	float g() const { return m_g; }
	float b() const { return m_b; }
	float a() const { return m_a; }

	void setR(float _r) { m_r = _r; }
	void setG(float _g) { m_g = _g; }
	void setB(float _b) { m_b = _b; }
	void setA(float _a) { m_a = _a; }

	Colour operator+(Colour const& _c) const { return Colour(m_r + _c.m_r, m_g + _c.m_g, m_b + _c.m_b, m_a + _c.m_a); }
	Colour operator*(float _f) const { return Colour(m_r * _f, m_g * _f, m_b * _f, m_a); }
	Colour operator/(float _f) const { return Colour(m_r / _f, m_g / _f, m_b / _f, m_a); }
	
	void writeRgba(uint8_t* _dest) const { _dest[0] = uint8_t(m_r * 255.f); _dest[1] = uint8_t(m_g * 255.f); _dest[2] = uint8_t(m_b * 255.f); _dest[3] = uint8_t(m_a * 255.f); }
	
	static const Colour Black;
	static const Colour White;
	static const Colour Red;
	static const Colour Orange;
	static const Colour Yellow;
	static const Colour Green;
	static const Colour Blue;
	static const Colour Indigo;
	static const Colour Violet;
	
private:	
	float m_r;
	float m_g;
	float m_b;
	float m_a;
};

}
