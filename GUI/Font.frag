precision mediump float;
uniform vec4 u_color;
uniform sampler2D u_tex;
varying vec2 v_texCoord;

void main(void)
{
	gl_FragColor.rgba = texture2D(u_tex, v_texCoord).a * u_color.rgba;
}
