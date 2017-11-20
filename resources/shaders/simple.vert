
#version 150
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
// uniform mat4 NormalMatrix;
uniform vec3 LightOrigin;
uniform vec3 Color;

out vec3 normal;
out vec3 light;
out vec3 view;
out vec3 color;

void main (void) {
	gl_Position = 
		ProjectionMatrix * ViewMatrix * 
		ModelMatrix * vec4(in_Position, 1.0);
	vec3 fragment = 
		(ModelMatrix * vec4(in_Position, 1.0)).xyz;
	vec3 camera = 
		(inverse(ProjectionMatrix * ViewMatrix) * 
		vec4(0,0,0, 1.0f)).xyz;
	view   = normalize(camera - fragment);
	light  = normalize(LightOrigin - fragment);
	normal = normalize((ModelMatrix * vec4(in_Normal, .0f)).xyz);
	color  = Color;
}
