uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_texCoord;

void main()
{
    gl_Position.zw = vec2(1.0);
    gl_Position.xy = a_texCoordPosition.zw;
    v_texCoord = a_texCoordPosition.xy;
    // TODO: consider getting rid of this...
    gl_Position.xy = gl_Position.xy / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
}

