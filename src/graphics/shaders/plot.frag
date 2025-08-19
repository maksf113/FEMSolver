#version 450 core
in vec3 f_in_normal;
in float f_in_val; // normalized solution

layout (location = 0) out vec4 fragColor;

layout (std140, binding = 0) uniform u_data
{
	mat4 view;
	mat4 projection;
	bool is2D;
	int labelCount;
	int drawIsolines;
};

uniform sampler1D u_colorMap;

uniform vec3 lightDir = vec3(-0.249, 0.498, -0.831);
uniform float ambient = 0.35;

void main()
{
	// --- lightning ---
	vec3 normal = normalize(f_in_normal);
	vec3 light = lightDir;
	float intensity = clamp(ambient + clamp(dot(normal, -light), 0.0, 1.0), 0.0, 1.0);
	//vec3 diffuseColor = f_in_col;
	vec3 diffuseColor = texture(u_colorMap, f_in_val).rgb;
	if(!is2D)
	{
		diffuseColor *= intensity;
	}
	// --- iso-lines ---
	float lineFactor = 0.0;
	if(drawIsolines == 1)
	{
		float lineInterval = 1.0 / float(labelCount);
		// value in terms of line intervals
		float v = f_in_val / lineInterval;
		// reate of change of 'v' across pixel
		float dv = fwidth(v);
		// distance to the nearest isoline [0.0, 0.5]
		float f = fract(v);
		float dist = min(f, 1.0 - f);

		lineFactor = 1.0 - smoothstep(dv * 0.1, dv * 1.0, dist);
	}
	vec3 lineColor = vec3(0.07, 0.07, 0.07);

	// --- final color ---
	vec3 mixedColor = mix(diffuseColor, lineColor, lineFactor * 0.4);
	fragColor = vec4(mixedColor, 1.0);
}