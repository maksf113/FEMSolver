#version 450 core
layout (location = 0) out vec4 fragColor;

uniform float u_alpha;

void main()
{
	fragColor = vec4(0.3, 0.3, 0.3, u_alpha);
}