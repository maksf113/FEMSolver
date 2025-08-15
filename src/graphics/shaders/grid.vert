#version 450 core

layout (location = 0) in vec3 v_in_pos;


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
}