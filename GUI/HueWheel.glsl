//@huewheel.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec2 geometry;
uniform vec4 offsetScale;

varying vec4 v_color;

vec4 hueToRgba(float hue)
{
	float h = hue * 6.0;
	if (h == 6.0)
		h = 0.0;

	float i = floor(h);
	float t = h - i;
	float q = 1.0 - t;
	t *= 0.3;				// or not
	float tone = 1.0 - t;	// or 1
	q *= 0.3;				// or not
	float qone = 1.0 - t;	// or 1
	if (i == 0.0)
		return vec4(tone, t, 0.0, 1.0);
	if (i == 1.0)
		return vec4(q, qone, 0.0, 1.0);
	if (i == 2.0)
		return vec4(0.0, tone, t, 1.0);
	if (i == 3.0)
		return vec4(0.0, q, qone, 1.0);
	if (i == 4.0)
		return vec4(t, 0.0, tone, 1.0);
	if (i == 5.0)
		return vec4(qone, 0.0, q, 1.0);
	return vec4(1.0, 0.0, 0.0, 1.0);
}

void main()
{
	v_color = (geometry.x != 0.0 && geometry.y != 0.0) ? hueToRgba(atan(geometry.x, geometry.y) / (2.0 * 3.14159265) + 0.5) : vec4(0.5, 0.5, 0.5, 1.0);
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = (offsetScale.xy + geometry * offsetScale.zw) / halfDisplaySize - sign(displaySize);
}

//@huewheel.frag
precision mediump float;
varying vec4 v_color;

void main()
{
	gl_FragColor = v_color;
}
