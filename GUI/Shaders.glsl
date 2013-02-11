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

//@colorize
precision mediump float;
uniform sampler2D u_tex;
uniform vec4 u_color;
uniform float u_overglow;
varying vec2 v_texCoord;
void main()
{
	float t = texture2D(u_tex, v_texCoord).r;
	gl_FragColor.a = u_color.a * t;
	gl_FragColor.rgb = mix(u_color.rgb, min(vec3(1), u_color.rgb + vec3(u_overglow)), t);
//	gl_FragColor.rgb = u_color.rgb + vec3(u_overglow * t);
//	gl_FragColor.rgb = texture2D(u_tex, v_texCoord).rgb * (vec3(u_overglow) + u_color.rgb);
//	gl_FragColor.a = u_color.a;
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
//@hblur4.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[6];
void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[ 0 ] = texCoord + vec2( -2.2884087325 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 1 ] = texCoord + vec2( -0.454966120801 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 2 ] = texCoord + vec2( 0.454966120801 * u_texturePitch.x, 0.0);
	v_blurTexCoords[ 3 ] = texCoord + vec2( 2.2884087325 * u_texturePitch.x, 0.0);
}

//@vblur4.vert
uniform vec2 u_texturePitch;
attribute vec2 a_position;
varying vec2 v_blurTexCoords[6];
void main()
{
	gl_Position.zw = vec2(1.0);
	gl_Position.xy = a_position.xy * 2.0 - vec2(1.0);
	vec2 texCoord = a_position.xy;
	v_blurTexCoords[ 0 ] = texCoord + vec2(0.0, -2.2884087325 * u_texturePitch.y);
	v_blurTexCoords[ 1 ] = texCoord + vec2(0.0, -0.454966120801 * u_texturePitch.y);
	v_blurTexCoords[ 2 ] = texCoord + vec2(0.0, 0.454966120801 * u_texturePitch.y);
	v_blurTexCoords[ 3 ] = texCoord + vec2(0.0, 2.2884087325 * u_texturePitch.y);
}

//@hblur4.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[6];
const float c = 0.0;
const float m = 1.4;
void main()
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0 ]) * (0.164602202961 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1 ]) * (0.442609282276 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2 ]) * (0.442609282276 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3 ]) * (0.164602202961 * m + c);
}

//@vblur4.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[6];
const float c = 0.2;
const float m = 1.0;
void main()
{
	gl_FragColor = vec4(0.0);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 0 ]) * (0.164602202961 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 1 ]) * (0.442609282276 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 2 ]) * (0.442609282276 * m + c);
	gl_FragColor += texture2D(u_tex, v_blurTexCoords[ 3 ]) * (0.164602202961 * m + c);
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



//-----------------------------------------------------------------------------
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

//@hblur8.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[8];
const float c = 0.2;
const float m = 0.8;

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

//@vblur8.frag
precision mediump float;
uniform sampler2D u_tex;
varying vec2 v_blurTexCoords[8];
const float c = 0.2;
const float m = 0.8;

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



