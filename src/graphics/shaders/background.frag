#version 450 core

in float f_in_clipY;

out vec4 fragColor;

uniform vec3 u_topColor = vec3(0.20, 0.20, 0.36);
uniform vec3 u_bottomColor = vec3(0.10, 0.12, 0.16);

void main()
{
	// map y coordinate from [-1,1] to [0, 1] for interpolation
	float t = (f_in_clipY + 1.0)/ 2.0;
	vec3 finalColor = mix(u_bottomColor, u_topColor, t);
	fragColor = vec4(finalColor, 1.0);
}