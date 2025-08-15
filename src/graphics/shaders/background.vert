#version 450 core

// full-screen quad
const vec2 positions[6] = vec2[](
	vec2(-1.0,  1.0),
	vec2(-1.0, -1.0),
	vec2( 1.0, -1.0),
	
	vec2( 1.0, -1.0),
	vec2( 1.0,  1.0),
	vec2(-1.0,  1.0)
);
// clip space y coordinate
out float f_in_clipY;

void main()
{
	gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
	f_in_clipY = positions[gl_VertexID].y;
}