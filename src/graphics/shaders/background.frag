#version 450 core

out vec4 fragColor;

uniform vec2 u_resolution;

uniform vec3 u_topColor = vec3(0.20, 0.20, 0.36);
uniform vec3 u_bottomColor = vec3(0.10, 0.12, 0.16);

uniform vec3 u_centerColor = vec3(0.25, 0.25, 0.45);
uniform vec3 u_edgeColor = vec3(0.10, 0.12, 0.16);
uniform float u_exponent = 2.0;

void main()
{
//	// vertical gradient
//	float t = gl_FragCoord.y / u_resolution.y;
//	vec3 finalColor = mix(u_bottomColor, u_topColor, t);

    // radial gradient
    // Get UV coordinates from [0,1]
    vec2 uv = gl_FragCoord.xy / u_resolution.xy;
    
    // Calculate distance from the center (0.5, 0.5)
    float d = distance(uv, vec2(0.5)) * 1.414; // sqrt of 2 to equal 1  in the corners
    float t = pow(d, u_exponent);
    // Mix colors based on distance
    vec3 finalColor = mix(u_centerColor, u_edgeColor, t); // Multiply d to control falloff
	fragColor = vec4(finalColor, 1.0);
}