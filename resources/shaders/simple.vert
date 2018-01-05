#version 430
#extension GL_ARB_explicit_attrib_location : require

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_TexCoord;
layout(location = 3) in vec3 in_Tangent;

uniform mat4 ModelMatrix;
uniform mat4 NormalMatrix;
uniform int  Shading;
uniform int  ID;

layout (std140, binding = 0) uniform CameraBlock {
  mat4 ViewMatrix;
  mat4 ProjectionMatrix;
} blockCam;


out vec3 normal;
out vec3 vertex;
out vec3 view;
out vec2 textCoord;
out vec3 tangent;
flat out int shading;
flat out int id;

void main (void) {
	vertex = (ModelMatrix * vec4(in_Position, 1)).xyz;
	vec3 camera = (inverse(blockCam.ViewMatrix) * vec4(0,0,0, 1)).xyz;
	// outs
	gl_Position = blockCam.ProjectionMatrix * blockCam.ViewMatrix * ModelMatrix * vec4(in_Position, 1);
	view      = normalize(camera - vertex);
	
	normal    = normalize((ModelMatrix * vec4(in_Normal, 0)).xyz);
	shading   = Shading;
	textCoord = in_TexCoord;
	tangent   = normalize((NormalMatrix * vec4(in_Tangent, 1)).xyz);
	id = ID;
}
