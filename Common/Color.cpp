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
#include "Colour.h"
#include "Color.h"
using namespace std;
using namespace Lightbox;

float Color::hueCorrection(float _h)
{
	static const std::map<float, float> c_brightnessCurve{{0, .9f}, {.166666, .75f}, {.333333, .85f}, {.5, .85f}, {.666666, 1.f}, {.8333333, .9f}, {1, .9f}};
	return lerpLookup(c_brightnessCurve, _h);
}

Color Color::fromRGB(std::array<float, 3> _rgb)
{
	Color ret(0, 0, 0);
	float m = min(_rgb[0], min(_rgb[1], _rgb[2]));
	ret.m_value = max(_rgb[0], max(_rgb[1], _rgb[2]));
	float c = ret.m_value - m;
	if (c != 0)
	{
		ret.m_sat = c / ret.m_value;
		array<float, 3> d;
		for (int i = 0; i < 3; ++i)
			d[i] = (((ret.m_value - _rgb[i]) / 6.f) + (c / 2.f)) / c;
		if (_rgb[0] == ret.m_value)
			ret.m_hue = d[2] - d[1];
		else if (_rgb[1] == ret.m_value)
			ret.m_hue = (1.f/3.f) + d[0] - d[2];
		else if (_rgb[2] == ret.m_value)
			ret.m_hue = (2.f/3.f) + d[1] - d[0];
		if (ret.m_hue < 0.f)
			ret.m_hue += 1.f;
		if (ret.m_hue > 1.f)
			ret.m_hue -= 1.f;
	}
	return ret;
}

Colour Color::toColour() const
{
	double      hh, p, q, t, ff;
	long        i;
	Colour         out;
	out.setA(m_alpha);

	if (m_sat <= 0.f)
	{       // < is bogus, just shuts up warnings
		if (isnan(m_hue))
		{   // m_hue == NAN
			out.setR(m_value);
			out.setG(m_value);
			out.setB(m_value);
			return out;
		}
		// error - should never happen
		out.setR(0.f);
		out.setG(0.f);
		out.setB(0.f);
		return out;
	}
	hh = m_hue;
	if (hh >= 1.f)
		hh = 0.f;
	hh *= 6.f;
	i = (long)hh;
	ff = hh - i;
	p = m_value * (1.f - m_sat);
	q = m_value * (1.f - (m_sat * ff));
	t = m_value * (1.f - (m_sat * (1.f - ff)));

	switch (i)
	{
	case 0:
		out.setR(m_value);
		out.setG(t);
		out.setB(p);
		break;
	case 1:
		out.setR(q);
		out.setG(m_value);
		out.setB(p);
		break;
	case 2:
		out.setR(p);
		out.setG(m_value);
		out.setB(t);
		break;

	case 3:
		out.setR(p);
		out.setG(q);
		out.setB(m_value);
		break;
	case 4:
		out.setR(t);
		out.setG(p);
		out.setB(m_value);
		break;
	default:
		out.setR(m_value);
		out.setG(p);
		out.setB(q);
		break;
	}
	return out;
}
