#version 450 core
in vec2 f_in_uv;
out vec4 color;

uniform sampler2D u_mask;
uniform vec3 u_color;
uniform float u_alpha;

void main()
{
	vec4 mask = vec4(1.0, 1.0, 1.0, texture(u_mask, f_in_uv).r);
	color = vec4(u_color, u_alpha) * mask;
}
