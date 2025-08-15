#version 450 core
layout (location = 0) in vec2 v_in_pos; // vec2 pos and vec2 tex coords
layout (location = 1) in vec2 v_in_uv;

out vec2 f_in_uv;

uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * vec4(v_in_pos, 0.0, 1.0);
	f_in_uv = v_in_uv;
}