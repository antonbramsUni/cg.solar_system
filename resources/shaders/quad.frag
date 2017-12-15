
#version 150

in vec2 textCoord;
uniform sampler2D Texture;
out vec4 out_Color;

void main () {
	out_Color = texture(Texture, textCoord);
}
