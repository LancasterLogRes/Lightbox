precision mediump float;
varying vec2 v_texCoord;

vec4 hueToRgba(float hue)
{
    float h = hue * 6.0;
    if (h == 6.0)
        h = 0.0;

    float i = floor(h);
    float t = h - i;
    float q = 1.0 - t;
    if (i == 0.0)
        return vec4(1.0, t, 0.0, 1.0);
    if (i == 1.0)
        return vec4(q, 1.0, 0.0, 1.0);
    if (i == 2.0)
        return vec4(0.0, 1.0, t, 1.0);
    if (i == 3.0)
        return vec4(0.0, q, 1.0, 1.0);
    if (i == 4.0)
        return vec4(t, 0.0, 1.0, 1.0);
    if (i == 5.0)
        return vec4(1.0, 0.0, q, 1.0);
}

void main()
{
    gl_FragColor = length(v_texCoord) < 1.0 ? hueToRgba(atan(v_texCoord.x, v_texCoord.y) / (2.0 * 3.14159265) + 0.5) : vec4(0.0, 0.0, 0.0, 0.0);
}
