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
uniform float ambient = 0.3;
uniform float lineHeight = 0.005;

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
		float normalizedValue = f_in_val;
		float lineInterval = 1.0 / float(labelCount);
		// f - position within isoline [0.0, 1.0]
  		float f = fract(normalizedValue / lineInterval);
		// distance to nearest isoline (f=0 and f=1)
		// dist = 0 on the isoline and dist = 0.5 in the middle between two isolines 
		float dist = min(f, 1.0 - f);
		// thickness factor accounting for plot steepness
		float steepnessFactor = 1.0 / clamp(normal.z * normal.z, 0.01, 1.0);
		float lineThickness = lineHeight * labelCount * steepnessFactor;
		// line factor = 1 on the isoline and fades to 0 at 0.5*thickness
		lineFactor = 1.0 - smoothstep(0.0, lineThickness / 2.0, dist);
	}
	vec3 lineColor = vec3(0.07, 0.07, 0.07);

	// --- final color ---
	vec3 mixedColor = mix(diffuseColor, lineColor, lineFactor * 0.4);
	fragColor = vec4(mixedColor, 1.0);
}