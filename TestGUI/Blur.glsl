//@hblur.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
	v_texCoord = a_texCoordPosition.xy;
	v_blurTexCoords[ 0] = v_texCoord + vec2(-7.0/displaySize.x, 0.0);
	v_blurTexCoords[ 1] = v_texCoord + vec2(-6.0/displaySize.x, 0.0);
	v_blurTexCoords[ 2] = v_texCoord + vec2(-5.0/displaySize.x, 0.0);
	v_blurTexCoords[ 3] = v_texCoord + vec2(-4.0/displaySize.x, 0.0);
	v_blurTexCoords[ 4] = v_texCoord + vec2(-3.0/displaySize.x, 0.0);
	v_blurTexCoords[ 5] = v_texCoord + vec2(-2.0/displaySize.x, 0.0);
	v_blurTexCoords[ 6] = v_texCoord + vec2(-1.0/displaySize.x, 0.0);
	v_blurTexCoords[ 7] = v_texCoord + vec2(1.0/displaySize.x, 0.0);
	v_blurTexCoords[ 8] = v_texCoord + vec2(2.0/displaySize.x, 0.0);
	v_blurTexCoords[ 9] = v_texCoord + vec2(3.0/displaySize.x, 0.0);
	v_blurTexCoords[10] = v_texCoord + vec2(4.0/displaySize.x, 0.0);
	v_blurTexCoords[11] = v_texCoord + vec2(5.0/displaySize.x, 0.0);
	v_blurTexCoords[12] = v_texCoord + vec2(6.0/displaySize.x, 0.0);
	v_blurTexCoords[13] = v_texCoord + vec2(7.0/displaySize.x, 0.0);
}

//@vblur.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
	v_texCoord = a_texCoordPosition.xy;
	v_blurTexCoords[ 0] = v_texCoord + vec2(0.0, -7.0/displaySize.x);
	v_blurTexCoords[ 1] = v_texCoord + vec2(0.0, -6.0/displaySize.x);
	v_blurTexCoords[ 2] = v_texCoord + vec2(0.0, -5.0/displaySize.x);
	v_blurTexCoords[ 3] = v_texCoord + vec2(0.0, -4.0/displaySize.x);
	v_blurTexCoords[ 4] = v_texCoord + vec2(0.0, -3.0/displaySize.x);
	v_blurTexCoords[ 5] = v_texCoord + vec2(0.0, -2.0/displaySize.x);
	v_blurTexCoords[ 6] = v_texCoord + vec2(0.0, -1.0/displaySize.x);
	v_blurTexCoords[ 7] = v_texCoord + vec2(0.0, 1.0/displaySize.x);
	v_blurTexCoords[ 8] = v_texCoord + vec2(0.0, 2.0/displaySize.x);
	v_blurTexCoords[ 9] = v_texCoord + vec2(0.0, 3.0/displaySize.x);
	v_blurTexCoords[10] = v_texCoord + vec2(0.0, 4.0/displaySize.x);
	v_blurTexCoords[11] = v_texCoord + vec2(0.0, 5.0/displaySize.x);
	v_blurTexCoords[12] = v_texCoord + vec2(0.0, 6.0/displaySize.x);
	v_blurTexCoords[13] = v_texCoord + vec2(0.0, 7.0/displaySize.x);
}

//@blur.frag
precision mediump float;
uniform vec2 displaySize;
uniform sampler2D u_tex;
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[14];

void main(void)
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0])*0.0044299121055113265;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1])*0.00895781211794;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2])*0.0215963866053;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3])*0.0443683338718;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 4])*0.0776744219933;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 5])*0.115876621105;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 6])*0.147308056121;
	gl_FragColor += texture2D(u_tex, v_texCoord         )*0.159576912161;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 7])*0.147308056121;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 8])*0.115876621105;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 9])*0.0776744219933;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[10])*0.0443683338718;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[11])*0.0215963866053;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[12])*0.00895781211794;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[13])*0.0044299121055113265;
}
