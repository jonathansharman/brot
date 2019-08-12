precision highp float;
precision highp vec2;

uniform vec2 res;
uniform vec2 pos;
uniform float scale;
uniform int max_iters;
uniform vec3 palette;

void main() {
	float height = scale;
	float width = height * res.x / res.y;

	vec2 c = pos;
	c.x += width * ((gl_FragCoord.x - 1.0) / res.x - 0.5);
	c.y += height * ((1.0 - (gl_FragCoord.y - 1.0) / res.y) - 0.5);

	int iters = 0;
	vec2 z = c;
	while (iters < max_iters && dot(z, z) < 4.0) {
		z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
		++iters;
	}

	if (iters == max_iters) {
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	} else {
		float lum = 10.0 * float(iters) / max_iters;
		gl_FragColor = vec4(palette.r * lum, palette.g * lum, palette.b * lum, 1.0);
	}
}
