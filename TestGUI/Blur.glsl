//@hblur.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_blurTexCoords[10];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
	vec2 texCoord = a_texCoordPosition.xy;
	v_blurTexCoords[0] = texCoord + vec2(-8.352848141159328/displaySize.x, 0.0);
	v_blurTexCoords[1] = texCoord + vec2(-6.386078308641078/displaySize.x, 0.0);
	v_blurTexCoords[2] = texCoord + vec2(-4.420404985451333/displaySize.x, 0.0);
	v_blurTexCoords[3] = texCoord + vec2(-2.455519138247958/displaySize.x, 0.0);
	v_blurTexCoords[4] = texCoord + vec2(-0.4910811930242767/displaySize.x, 0.0);
	v_blurTexCoords[5] = texCoord + vec2(0.4910811930242767/displaySize.x, 0.0);
	v_blurTexCoords[6] = texCoord + vec2(2.455519138247958/displaySize.x, 0.0);
	v_blurTexCoords[7] = texCoord + vec2(4.420404985451333/displaySize.x, 0.0);
	v_blurTexCoords[8] = texCoord + vec2(6.386078308641078/displaySize.x, 0.0);
	v_blurTexCoords[9] = texCoord + vec2(8.352848141159328/displaySize.x, 0.0);
}

//@vblur.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_blurTexCoords[10];

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
	vec2 texCoord = a_texCoordPosition.xy;
	v_blurTexCoords[0] = texCoord + vec2(0.0, -8.352848141159328/displaySize.y);
	v_blurTexCoords[1] = texCoord + vec2(0.0, -6.386078308641078/displaySize.y);
	v_blurTexCoords[2] = texCoord + vec2(0.0, -4.420404985451333/displaySize.y);
	v_blurTexCoords[3] = texCoord + vec2(0.0, -2.455519138247958/displaySize.y);
	v_blurTexCoords[4] = texCoord + vec2(0.0, -0.4910811930242767/displaySize.y);
	v_blurTexCoords[5] = texCoord + vec2(0.0, 0.4910811930242767/displaySize.y);
	v_blurTexCoords[6] = texCoord + vec2(0.0, 2.455519138247958/displaySize.y);
	v_blurTexCoords[7] = texCoord + vec2(0.0, 4.420404985451333/displaySize.y);
	v_blurTexCoords[8] = texCoord + vec2(0.0, 6.386078308641078/displaySize.y);
	v_blurTexCoords[9] = texCoord + vec2(0.0, 8.352848141159328/displaySize.y);
}

//@blur.frag
precision mediump float;
uniform vec2 displaySize;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[10];

void main(void)
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0])*0.01697073243489202;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1])*0.04858020688877497;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2])*0.10503926176525383;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3])*0.17156791008540617;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 4])*0.21171468001085988;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 5])*0.21171468001085988;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 6])*0.17156791008540617;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 7])*0.10503926176525383;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 8])*0.04858020688877497;
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 9])*0.01697073243489202;
}


//@shblur.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[10];
const float offset[10] = float[10](-8.352848141159328, -6.386078308641078, -4.420404985451333, -2.455519138247958, -0.4910811930242767, 0.4910811930242767, 2.455519138247958, 4.420404985451333, 6.386078308641078, 8.352848141159328);

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
	v_texCoord = a_texCoordPosition.xy;
	for (int i = 0; i < 10; ++i)
		v_blurTexCoords[i] = v_texCoord + vec2(offset[i]/displaySize.x, 0.0);
}

//@svblur.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_texCoord;
varying vec2 v_blurTexCoords[10];
const float offset[10] = float[10](-8.352848141159328, -6.386078308641078, -4.420404985451333, -2.455519138247958, -0.4910811930242767, 0.4910811930242767, 2.455519138247958, 4.420404985451333, 6.386078308641078, 8.352848141159328);

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
	v_texCoord = a_texCoordPosition.xy;
	for (int i = 0; i < 10; ++i)
		v_blurTexCoords[i] = v_texCoord + vec2(0.0, offset[i]/displaySize.y);
}

//@sblur.frag
precision mediump float;
uniform vec2 displaySize;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[10];

const float weight[10] = float[10](0.01697073243489202, 0.04858020688877497, 0.10503926176525383, 0.17156791008540617, 0.21171468001085988, 0.21171468001085988, 0.17156791008540617, 0.10503926176525383, 0.04858020688877497, 0.01697073243489202);

void main(void)
{
	gl_FragColor = vec4(0.0);
	for (int i = 0; i < 10; ++i)
		gl_FragColor += texture2D(u_tex, v_blurTexCoords[ i]) * offset[i];
}
