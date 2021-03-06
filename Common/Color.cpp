/* BEGIN COPYRIGHT
 *
 * This file is part of Noted.
 *
 * Copyright ©2011, 2012, Lancaster Logic Response Limited.
 *
 * Noted is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Noted is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Noted.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include "RGBA.h"
#include "Color.h"
using namespace std;
using namespace lb;

float Color::hueCorrection(float _h)
{
	static const std::map<float, float> c_brightnessCurve{{0, .9f}, {.166666, .75f}, {.333333, .85f}, {.5, .85f}, {.666666, 1.f}, {.8333333, .9f}, {1, .9f}};
	return lerpLookup(c_brightnessCurve, _h);
}

Color Color::withConstantLight(float _hue, float _sat, float _value, float _alpha)
{
	float ipart;
	float fpart = modff(_hue / .16666f, &ipart);
	float maxTotal = lerp(_sat, 2.f, (int)ipart % 2 ? 2 - fpart : (1 + fpart));
	return Color(_hue, _sat, _value / maxTotal, _alpha);
}

inline int fromHex(char _a, char _b)
{
	if (_a != '0')
		return fromHex('0', _a) * 16 + fromHex('0', _b);
	else
		return (_b >= '0' && _b <= '9') ? _b - '0' : (_b >= 'a' && _b <= 'f') ? _b - 'a' + 10 : (_b >= 'A' && _b <= 'F') ? _b - 'A' + 10 : 0;
}

Color Color::interpret(string const& _s)
{
	if (_s.size() > 1 && _s[0] == '#')
	{
		if (_s.size() == 4)
			return Color(RGB8Space, fromHex(_s[1], _s[1]), fromHex(_s[2], _s[2]), fromHex(_s[3], _s[3]));
		else if (_s.size() == 5)
			return Color(RGBA8Space, fromHex(_s[1], _s[1]), fromHex(_s[2], _s[2]), fromHex(_s[3], _s[3]), fromHex(_s[4], _s[4]));
		else if (_s.size() == 7)
			return Color(RGB8Space, fromHex(_s[1], _s[2]), fromHex(_s[3], _s[4]), fromHex(_s[5], _s[6]));
		else if (_s.size() == 9)
			return Color(RGBA8Space, fromHex(_s[1], _s[2]), fromHex(_s[3], _s[4]), fromHex(_s[5], _s[6]), fromHex(_s[7], _s[8]));
	}
	return Color();
}

void Color::convertFrom(ColorSpace _cc, float _x, float _y, float _z, float _w)
{
	if (_cc == RGBA8Space || _cc == RGB8Space)
		_x /= 255, _y /= 255, _z /= 255, _w /= 255;

	m_alpha = 1.f;
	switch (_cc)
	{
	case RGBASpace:
	case RGBA8Space:
		m_alpha = _w;
	case RGBSpace:
	case RGB8Space:
	{
		float m = min(_x, min(_y, _z));
		m_value = max(_x, max(_y, _z));
		float c = m_value - m;
		if (c != 0)
		{
			m_sat = c / m_value;
			array<float, 3> d = {{ _x, _y, _z }};
			for (float& i: d)
				i = (((m_value - i) / 6.f) + (c / 2.f)) / c;
			if (_x == m_value)
				m_hue = d[2] - d[1];
			else if (_y == m_value)
				m_hue = (1.f/3.f) + d[0] - d[2];
			else if (_z == m_value)
				m_hue = (2.f/3.f) + d[1] - d[0];
			if (m_hue < 0.f)
				m_hue += 1.f;
			if (m_hue > 1.f)
				m_hue -= 1.f;
		}
		break;
	}
	case HSLSpace:
		m_hue = _x;
		m_sat = _x >= 0 ? _y * clamp(2.f - _z * 2.f, 0.f, 1.f) : 0.f;
		m_value = _x >= 0 ? lerp(_y, _z, clamp(_z * 2.f, 0.f, 1.f)) : _z;
		break;
	}
}

fVector4 Color::convertTo(ColorSpace _cc) const
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 1.f;

	switch (_cc)
	{
	case RGBASpace:
	case RGBA8Space:
		w = m_alpha;
	case RGBSpace:
	case RGB8Space:
	{
		if (m_sat <= 0.f)
		{       // < is bogus, just shuts up warnings
			if (isnan(m_hue))
			{
				// error - should never happen
				x = 0.f;
				y = 0.f;
				z = 0.f;
				break;
			}
			// m_hue == NAN
			x = m_value;
			y = m_value;
			z = m_value;
			break;
		}
		float hh = m_hue;
		if (hh >= 1.f)
			hh = 0.f;
		hh *= 6.f;
		int i = hh;
		float ff = hh - i;
		float p = m_value * (1.f - m_sat);
		float q = m_value * (1.f - (m_sat * ff));
		float t = m_value * (1.f - (m_sat * (1.f - ff)));

		switch (i)
		{
		case 0:
			x = m_value;
			y = t;
			z = p;
			break;
		case 1:
			x = q;
			y = m_value;
			z = p;
			break;
		case 2:
			x = p;
			y = m_value;
			z = t;
			break;
		case 3:
			x = p;
			y = q;
			z = m_value;
			break;
		case 4:
			x = t;
			y = p;
			z = m_value;
			break;
		default:
			x = m_value;
			y = p;
			z = q;
			break;
		}
		break;
	}
	default:;
	}
	if (_cc == RGBA8Space || _cc == RGB8Space)
		x *= 255, y *= 255, z *= 255, w *= 255;
	return fVector4(x, y, z, w);
}
