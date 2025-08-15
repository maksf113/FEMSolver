#version 450 core

layout (location = 0) in vec3 v_in_pos;
layout (location = 1) in vec3 v_in_normal;
layout (location = 2) in float v_in_val; // normalized solution

out vec3 f_in_normal;
out float f_in_val;

layout (std140, binding = 0) uniform u_data
{
	mat4 view;
	mat4 projection;
	bool is2D;
	int labelCount;
	int drawIsolines;
};

uniform mat4 u_model;

void main()
{
	gl_Position = projection * view * u_model * vec4(v_in_pos, 1.0);
	f_in_normal = v_in_normal;
	f_in_val = v_in_val;
}