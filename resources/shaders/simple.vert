
#version 150
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 LightOrigin;
uniform int  Shading;
uniform int  ID;

out vec3 normal;
out vec3 light;
out vec3 view;
out vec2 pass_TexCoord;
flat out int shading;
flat out int id;

void main (void) {
	vec3 vertex = (ModelMatrix * vec4(in_Position, 1)).xyz;
	vec3 camera = (inverse(ViewMatrix) * vec4(0,0,0, 1)).xyz;
	// outs
	gl_Position = 
		ProjectionMatrix * ViewMatrix * 
		ModelMatrix * vec4(in_Position, 1);
	view    = normalize(camera - vertex);
	light   = normalize(LightOrigin - vertex);
	normal  = normalize((ModelMatrix * vec4(in_Normal, 0)).xyz);
	shading = Shading;
	pass_TexCoord = in_TexCoord;
	id = ID;
}
