uniform vec2 displaySize;
vec2 halfDisplaySize = vec2(displaySize.x, -displaySize.y) / vec2(2.0);

attribute vec2 a_index;
attribute vec2 a_layout;
attribute vec2 a_source;
attribute vec2 a_size;
varying vec2 v_texCoord;

void main()
{
    gl_Position.zw = vec2(1.0);
    gl_Position.xy = a_layout + a_size * a_index;
    v_texCoord = (a_source + a_size * a_index) / 512.0;
    gl_Position.xy = gl_Position.xy / halfDisplaySize - vec2(1.0, -1.0);    // transform pixels -> clip space.
}

