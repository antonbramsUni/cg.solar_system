
#version 150
#extension GL_ARB_explicit_attrib_location : require

layout(location = 0) in vec3 in_Position;
out vec2 textCoord;

void main (void) {
	// gl_Position = vec4(
	// 	in_Position.x, 
	// 	in_Position.y, 
	// 	in_Position.z, 1.0);
	gl_Position = vec4(0.0,0.0,0.0,0.0);
	textCoord = vec2(
		in_Position.x + 1,
		in_Position.y + 1) / 2;
}
