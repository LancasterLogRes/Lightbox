precision mediump float;
uniform vec3 u_color;
uniform sampler2D u_tex;
varying vec2 v_texCoord;

void main(void)
{
    gl_FragColor.a = texture2D(u_tex, v_texCoord).a;
    gl_FragColor.rgb = u_color;
}
