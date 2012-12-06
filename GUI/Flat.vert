uniform vec2 displaySize;
vec2 halfDisplaySize = vec2(displaySize.x, -displaySize.y) / vec2(2.0);

attribute vec2 geometry;
uniform vec4 offsetScale;

void main()
{
    gl_Position.zw = vec2(1.0);
    gl_Position.xy = (offsetScale.xy + geometry * offsetScale.zw) / halfDisplaySize - vec2(1.0, -1.0);
}
