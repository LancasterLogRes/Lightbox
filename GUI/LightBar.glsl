//@lightbar.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec2 geometry;
uniform vec4 offsetScale;

uniform vec4 topColor;
uniform vec4 bottomColor;

varying vec4 v_color;

void main()
{
    v_color = (geometry.y == 0.0) ? topColor : bottomColor;
    gl_Position.zw = vec2(1.0);
    gl_Position.xy = (offsetScale.xy + geometry * offsetScale.zw) / halfDisplaySize - sign(displaySize);
}

//@lightbar.frag
precision mediump float;
varying vec4 v_color;

void main()
{
	gl_FragColor = v_color;
}
