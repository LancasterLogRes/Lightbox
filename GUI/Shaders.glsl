//@view.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec2 geometry;
uniform vec4 color;
uniform vec4 offsetScale;
uniform float gradient;

varying vec4 v_color;

void main()
{
	v_color.rgb = color.rgb + vec3(gradient * (geometry.y * 2.0 - 1.0));
	v_color.a = color.a;
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = (offsetScale.xy + geometry * offsetScale.zw) / halfDisplaySize - sign(displaySize);
}
//@view.frag
precision mediump float;
varying vec4 v_color;

void main()
{
	gl_FragColor = v_color;
}


//-----------------------------------------------------------------------------
//@font.vert
uniform vec2 displaySize;
uniform vec2 u_texSize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec2 a_index;
attribute vec2 a_layout;
attribute vec2 a_source;
attribute vec2 a_size;
varying vec2 v_texCoord;

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_layout + a_size * a_index;
	v_texCoord = (a_source + a_size * a_index) / u_texSize;
	gl_Position.xy = gl_Position.xy / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
}
//@font.frag
precision mediump float;
uniform vec4 u_color;
uniform sampler2D u_tex;
varying vec2 v_texCoord;

void main()
{
//	gl_FragColor.rgba = texture2D(u_tex, v_texCoord).a * u_color.rgba;
	gl_FragColor.a = texture2D(u_tex, v_texCoord).a * u_color.a;
	gl_FragColor.rgb = u_color.rgb;
}


//-----------------------------------------------------------------------------
//@flat.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec2 geometry;
uniform vec4 offsetScale;

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = (offsetScale.xy + geometry * offsetScale.zw) / halfDisplaySize - sign(displaySize);
}
//@flat.frag
precision mediump float;
uniform vec4 color;

void main()
{
	gl_FragColor = color;
}


//-----------------------------------------------------------------------------
//@texture.vert
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
//@texture.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_texCoord;

void main()
{
//    gl_FragColor.rgba = vec4(1.0, 1.0, 1.0, 0.1);
	gl_FragColor.rgba = texture2D(u_tex, v_texCoord);
}


//-----------------------------------------------------------------------------
//@general.vert
uniform vec2 displaySize;
vec2 halfDisplaySize = displaySize / vec2(2.0);

attribute vec4 a_texCoordPosition;
varying vec2 v_texCoord;

void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_texCoordPosition.zw;
	v_texCoord = a_texCoordPosition.xy;
	gl_Position.xy = gl_Position.xy / halfDisplaySize - sign(displaySize);    // transform pixels -> clip space.
}
//@general.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_texCoord;

void main()
{
	gl_FragColor.rgba = texture2D(u_tex, v_texCoord);
}


//-----------------------------------------------------------------------------
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

//@hblur6.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[6];
const float c = 0.0;
const float m = 1.4;
void main()
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0 ]) * (0.0663119730067 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1 ]) * (0.1952704764 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2 ]) * (0.310086813508 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3 ]) * (0.310086813508 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 4 ]) * (0.1952704764 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 5 ]) * (0.0663119730067 * m + c);
}

//@vblur6.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[6];
const float c = 0.2;
const float m = 1.0;
void main()
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0 ]) * (0.0663119730067 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1 ]) * (0.1952704764 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2 ]) * (0.310086813508 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3 ]) * (0.310086813508 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 4 ]) * (0.1952704764 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 5 ]) * (0.0663119730067 * m + c);
}


//-----------------------------------------------------------------------------
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

