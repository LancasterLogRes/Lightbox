uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec2 geometry;
uniform vec4 color;
uniform vec4 offsetScale;
uniform float gradient;

varying vec4 v_color;

void main()
{
    v_color.rgb = color.rgb * vec3(gradient * (geometry.y * 2.0 - 1.0) + 1.0);
    v_color.a = color.a;
    gl_Position.zw = vec2(1.0);
    gl_Position.xy = (offsetScale.xy + geometry * offsetScale.zw) / halfDisplaySize - sign(displaySize);
}
