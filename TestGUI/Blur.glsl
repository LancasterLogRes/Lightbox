//@hblur10.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[10];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[0] = texCoord + vec2(-8.352848141159328*u_texturePitch.x, 0.0);
	v_blurTexCoords[1] = texCoord + vec2(-6.386078308641078*u_texturePitch.x, 0.0);
	v_blurTexCoords[2] = texCoord + vec2(-4.420404985451333*u_texturePitch.x, 0.0);
	v_blurTexCoords[3] = texCoord + vec2(-2.455519138247958*u_texturePitch.x, 0.0);
	v_blurTexCoords[4] = texCoord + vec2(-0.4910811930242767*u_texturePitch.x, 0.0);
	v_blurTexCoords[5] = texCoord + vec2(0.4910811930242767*u_texturePitch.x, 0.0);
	v_blurTexCoords[6] = texCoord + vec2(2.455519138247958*u_texturePitch.x, 0.0);
	v_blurTexCoords[7] = texCoord + vec2(4.420404985451333*u_texturePitch.x, 0.0);
	v_blurTexCoords[8] = texCoord + vec2(6.386078308641078*u_texturePitch.x, 0.0);
	v_blurTexCoords[9] = texCoord + vec2(8.352848141159328*u_texturePitch.x, 0.0);
}

//@vblur10.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[10];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[0] = texCoord + vec2(0.0, -8.352848141159328*u_texturePitch.y);
	v_blurTexCoords[1] = texCoord + vec2(0.0, -6.386078308641078*u_texturePitch.y);
	v_blurTexCoords[2] = texCoord + vec2(0.0, -4.420404985451333*u_texturePitch.y);
	v_blurTexCoords[3] = texCoord + vec2(0.0, -2.455519138247958*u_texturePitch.y);
	v_blurTexCoords[4] = texCoord + vec2(0.0, -0.4910811930242767*u_texturePitch.y);
	v_blurTexCoords[5] = texCoord + vec2(0.0, 0.4910811930242767*u_texturePitch.y);
	v_blurTexCoords[6] = texCoord + vec2(0.0, 2.455519138247958*u_texturePitch.y);
	v_blurTexCoords[7] = texCoord + vec2(0.0, 4.420404985451333*u_texturePitch.y);
	v_blurTexCoords[8] = texCoord + vec2(0.0, 6.386078308641078*u_texturePitch.y);
	v_blurTexCoords[9] = texCoord + vec2(0.0, 8.352848141159328*u_texturePitch.y);
}

//@blur10.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[10];

void main(void)
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[0])*0.01697073243489202;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[1])*0.04858020688877497;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[2])*0.10503926176525383;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[3])*0.17156791008540617;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[4])*0.21171468001085988;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[5])*0.21171468001085988;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[6])*0.17156791008540617;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[7])*0.10503926176525383;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[8])*0.04858020688877497;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[9])*0.01697073243489202;
}


//@hblur6.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[6];
void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[ 0 ] = texCoord + vec2( -4.32684270363 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 1 ] = texCoord + vec2( -2.40097869019 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 2 ] = texCoord + vec2( -0.479941326778 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 3 ] = texCoord + vec2( 0.479941326778 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 4 ] = texCoord + vec2( 2.40097869019 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 5 ] = texCoord + vec2( 4.32684270363 * u_texturePitch.x, 0.0);
}

//@vblur6.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[6];
void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[ 0 ] = texCoord + vec2(0.0, -4.32684270363 * u_texturePitch.y);
	v_blurTexCoords[ 1 ] = texCoord + vec2(0.0, -2.40097869019 * u_texturePitch.y);
	v_blurTexCoords[ 2 ] = texCoord + vec2(0.0, -0.479941326778 * u_texturePitch.y);
	v_blurTexCoords[ 3 ] = texCoord + vec2(0.0, 0.479941326778 * u_texturePitch.y);
	v_blurTexCoords[ 4 ] = texCoord + vec2(0.0, 2.40097869019 * u_texturePitch.y);
	v_blurTexCoords[ 5 ] = texCoord + vec2(0.0, 4.32684270363 * u_texturePitch.y);
}

//@blur6.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[6];
const float c = 0.1;
const float m = 1.2;
void main(void)
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0 ]) * (0.0663119730067 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1 ]) * (0.1952704764 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2 ]) * (0.310086813508 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3 ]) * (0.310086813508 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 4 ]) * (0.1952704764 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 5 ]) * (0.0663119730067 * m + c);
}



//@hblur8.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[8];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[ 0 ] = texCoord + vec2( -6.35731631295 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 1 ] = texCoord + vec2( -4.39977414722 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 2 ] = texCoord + vec2( -2.44379325662 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 3 ] = texCoord + vec2( -0.488712855391 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 4 ] = texCoord + vec2( 0.488712855391 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 5 ] = texCoord + vec2( 2.44379325662 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 6 ] = texCoord + vec2( 4.39977414722 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 7 ] = texCoord + vec2( 6.35731631295 * u_texturePitch.x, 0.0);
}

//@vblur8.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[8];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[ 0 ] = texCoord + vec2(0.0, -6.35731631295 * u_texturePitch.y);
	v_blurTexCoords[ 1 ] = texCoord + vec2(0.0, -4.39977414722 * u_texturePitch.y);
	v_blurTexCoords[ 2 ] = texCoord + vec2(0.0, -2.44379325662 * u_texturePitch.y);
	v_blurTexCoords[ 3 ] = texCoord + vec2(0.0, -0.488712855391 * u_texturePitch.y);
	v_blurTexCoords[ 4 ] = texCoord + vec2(0.0, 0.488712855391 * u_texturePitch.y);
	v_blurTexCoords[ 5 ] = texCoord + vec2(0.0, 2.44379325662 * u_texturePitch.y);
	v_blurTexCoords[ 6 ] = texCoord + vec2(0.0, 4.39977414722 * u_texturePitch.y);
	v_blurTexCoords[ 7 ] = texCoord + vec2(0.0, 6.35731631295 * u_texturePitch.y);
}

//@blur8.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[8];
const float c = 0.12;
const float m = 1.0;

void main(void)
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0 ]) * (0.0370939541016 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1 ]) * (0.0979959017165 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2 ]) * (0.181810802291 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3 ]) * (0.236938426849 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 4 ]) * (0.236938426849 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 5 ]) * (0.181810802291 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 6 ]) * (0.0979959017165 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 7 ]) * (0.0370939541016 * m + c);
}



//@pass.vert
uniform vec2 u_texturePitch;
uniform vec2 u_offset;
attribute vec2 a_position;
varying vec2 v_texCoord;
void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	v_texCoord = a_position.xy + u_texturePitch * u_offset;
}

//@pass.frag
precision mediump float;
uniform sampler2D u_tex;
uniform float u_amplitude;
varying vec2 v_texCoord;
void main()
{
	gl_FragColor = texture2D(u_tex, v_texCoord) * (1.0 + u_amplitude);
}

