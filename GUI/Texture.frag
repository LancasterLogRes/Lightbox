precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_texCoord;

void main(void)
{
//    gl_FragColor.rgba = vec4(1.0, 1.0, 1.0, 0.1);
    gl_FragColor.rgba = texture2D(u_tex, v_texCoord);
}
